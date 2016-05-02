/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *
 ****************************************************************************/

#include "WirelessChannel.h"

Define_Module(WirelessChannel);

int WirelessChannel::numInitStages() const
{
	return 2;
}

void WirelessChannel::initialize(int stage)
{
	if (stage == 0) {
		readIniFileParameters();
		return;
	}

	/* variable to report initialization run time */
	clock_t startTime;
	startTime = clock();

	/****************************************************
	 * To handle mobile nodes we break up the space into
	 * cells. All path loss calculations are now done
	 * between cells. First we need to find out how many
	 * distinct cells we have in space, based on the
	 * dimensions of the space and the declared cell size.
	 * If we only have static nodes, then we keep the
	 * same variables and abstractions to make the code
	 * more compact and easier to read, but we do not
	 * really need to divide the space into cells. Even
	 * though we keep the variable names, cells in the
	 * static case only correspond to node positions
	 * and we only have numOfNodes cells, much like
	 * we used to do in Castalia 1.3
	 ****************************************************/

	if (onlyStaticNodes) {
		numOfSpaceCells = numOfNodes;
	} else {
		if (xFieldSize <= 0) {
			xFieldSize = 1;
			xCellSize = 1;
		}
		if (yFieldSize <= 0) {
			yFieldSize = 1;
			yCellSize = 1;
		}
		if (zFieldSize <= 0) {
			zFieldSize = 1;
			zCellSize = 1;
		}
		if (xCellSize <= 0)
			xCellSize = xFieldSize;
		if (yCellSize <= 0)
			yCellSize = yFieldSize;
		if (zCellSize <= 0)
			zCellSize = zFieldSize;

		numOfZCells = (int)ceil(zFieldSize / zCellSize);
		numOfYCells = (int)ceil(yFieldSize / yCellSize);
		numOfXCells = (int)ceil(xFieldSize / xCellSize);
		numOfSpaceCells = numOfZCells * numOfYCells * numOfXCells;

	   /***************************************************************
	    * Calculate some values that  help us transform a 1D index in
	    * [0..numOfSpaceCells -1] to a 3D index x, y, z and vice versa.
	    * Each variable holds index increments (in the 1D large index)
	    * needed to move one space cell in the z, y, and x directions
	    **************************************************************/
		zIndexIncrement = numOfYCells * numOfXCells;
		yIndexIncrement = numOfXCells;
		xIndexIncrement = 1;
	}

	/***************************************************************
	 * Allocate and initialize cellOccupation and nodeLocation.
	 * nodeLocation keeps the state about all nodes locations and
	 * cellOccupation is an array of lists. List at index i contains
	 * the node IDs that reside in cell i. We define and use these
	 * arrays even for the static nodes case as it makes the code
	 * more compact and easier to follow.
	 **************************************************************/
	nodeLocation = new NodeLocation_type[numOfNodes];
	if (nodeLocation == NULL)
		opp_error("Could not allocate array nodeLocation\n");

	cellOccupation = new list<int>[numOfSpaceCells];
	if (cellOccupation == NULL)
		opp_error("Could not allocate array cellOccupation\n");

	cTopology *topo;	// temp variable to access initial location of the nodes
	topo = new cTopology("topo");
	topo->extractByNedTypeName(cStringTokenizer("wsn_lighting.node.Node").asVector());

	for (int i = 0; i < numOfNodes; i++) {
		VirtualMobilityManager *nodeMobilityModule =
			check_and_cast<VirtualMobilityManager*>
			(topo->getNode(i)->getModule()->getSubmodule("MobilityManager"));
		nodeLocation[i] = nodeMobilityModule->getLocation();
		nodeLocation[i].cell = i;

		if (!onlyStaticNodes) {
			/******************************************************************
			 * Compute the cell this node is in and initialize cellOccupation.
			 * Cavaet in computing the XYZ indices:
			 * Because we allow cell resolutions that do not perfectly divide
			 * the field (we take the ceiling of the division when calculating
			 * numOfXCells) this means that the edge cells might be smaller than
			 * the others. So in some cases, the calculation we are doing
			 * below, might give the wrong cell by +1. That's why we are doing
			 * the test immediately after.
			 ******************************************************************/
			int xIndex = (int)floor(nodeLocation[i].x / xFieldSize * numOfXCells);
			if (((xIndex - 1) * xCellSize) >= nodeLocation[i].x)
				xIndex--;
			else if (xIndex >= numOfXCells) {
				xIndex = numOfXCells - 1;	// the maximum possible x index
				if (nodeLocation[i].x > xFieldSize)
					trace() << "WARNING at initialization: node position out of bounds in X dimension!\n";
			}

			int yIndex = (int)floor(nodeLocation[i].y / yFieldSize * numOfYCells);
			if (((yIndex - 1) * yCellSize) >= nodeLocation[i].y)
				yIndex--;
			else if (yIndex >= numOfYCells) {
				yIndex = numOfYCells - 1;	// the maximum possible y index
				if (nodeLocation[i].y > yFieldSize)
					trace() << "WARNING at initialization: node position out of bounds in Y dimension!\n";
			}

			int zIndex = (int)floor(nodeLocation[i].z / zFieldSize * numOfZCells);
			if (((zIndex - 1) * zCellSize) >= nodeLocation[i].z)
				zIndex--;
			else if (zIndex >= numOfZCells) {
				zIndex = numOfZCells - 1;	// the maximum possible z index
				if (nodeLocation[i].z > zFieldSize)
					trace() << "WARNING at initialization: node position out of bounds in Z dimension!\n";
			}

			int cell = zIndex * zIndexIncrement + yIndex * yIndexIncrement + xIndex * xIndexIncrement;
			if (cell < 0 || cell >= numOfSpaceCells) {
				opp_error("Cell out of bounds for node %i, please check your mobility module settings\n", i);
			}

			nodeLocation[i].cell = cell;
		}

		/*************************************************
		 * pushing ID i into the list cellOccupation[cell]
		 * (if onlyStaticNodes cell=i )
		 *************************************************/
		cellOccupation[nodeLocation[i].cell].push_front(i);
	}
	delete(topo);

	/**********************************************
	 * Allocate and initialize the pathLoss array.
	 * This is the "propagation map" of our space.
	 **********************************************/
	pathLoss = new list<PathLossElement*>[numOfSpaceCells];
	if (pathLoss == NULL)
		opp_error("Could not allocate array pathLoss\n");

	int elementSize = sizeof(PathLossElement) + 3 * sizeof(PathLossElement *);
	int totalElements = 0;	//keep track of pathLoss size for reporting purposes

	float x1, x2, y1, y2, z1, z2, dist;
	float PLd;		// path loss at distance dist, in dB
	float bidirectionalPathLossJitter; // variation of the pathloss in the two directions of a link, in dB

	/*******************************************************
	 * Calculate the distance, beyond which we cannot
	 * have connectivity between two nodes. This calculation is
	 * based on the maximum TXPower the signalDeliveryThreshold
	 * the pathLossExponent, the PLd0. For the random
	 * shadowing part we use 3*sigma to account for 99.7%
	 * of the cases. We use this value to considerably
	 * speed up the filling of the pathLoss array,
	 * especially for the mobile case.
	 *******************************************************/
	float distanceThreshold = d0 *
		pow(10.0,(maxTxPower - signalDeliveryThreshold - PLd0 + 3 * sigma) /
		(10.0 * pathLossExponent));

	for (int i = 0; i < numOfSpaceCells; i++) {
		if (onlyStaticNodes) {
			x1 = nodeLocation[i].x;
			y1 = nodeLocation[i].y;
			z1 = nodeLocation[i].z;
		} else {
			z1 = zCellSize * (int)floor(i / zIndexIncrement);
			y1 = yCellSize * (((int)floor(i / yIndexIncrement)) % zIndexIncrement);
			x1 = xCellSize * (((int)floor(i / xIndexIncrement)) % yIndexIncrement);
		}

		/* Path loss to yourself is 0.0 */
		pathLoss[i].push_front(new PathLossElement(i, 0.0));
		totalElements++;	//keep track of pathLoss size for reporting purposes

		for (int j = i + 1; j < numOfSpaceCells; j++) {
			if (onlyStaticNodes) {
				x2 = nodeLocation[j].x;
				y2 = nodeLocation[j].y;
				z2 = nodeLocation[j].z;
			} else {
				z2 = zCellSize * (int)(j / zIndexIncrement);
				y2 = yCellSize * (((int)(j / yIndexIncrement)) % zIndexIncrement);
				x2 = xCellSize * (((int)(j / xIndexIncrement)) % yIndexIncrement);

				if (fabs(x1 - x2) > distanceThreshold)
					continue;
				if (fabs(y1 - y2) > distanceThreshold)
					continue;
				if (fabs(z1 - z2) > distanceThreshold)
					continue;
			}

			dist = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
			if (dist > distanceThreshold)
				continue;

			/* if the distance is very small (arbitrarily: smaller than one tenth
			 * of the reference distance) then make the path loss 0dB
			 */
			if (dist < d0/10.0) {
				PLd = 0;
				bidirectionalPathLossJitter = 0;
			}
			else {
				PLd = PLd0 + 10.0 * pathLossExponent * log10(dist / d0) + normal(0, sigma);
				bidirectionalPathLossJitter = normal(0, bidirectionalSigma) / 2;
			}

			if (maxTxPower - PLd - bidirectionalPathLossJitter >= signalDeliveryThreshold) {
				pathLoss[i].push_front(new PathLossElement(j,PLd + bidirectionalPathLossJitter));
				totalElements++;	//keep track of pathLoss size for reporting purposes
			}

			if (maxTxPower - PLd + bidirectionalPathLossJitter >= signalDeliveryThreshold) {
				pathLoss[j].push_front(new PathLossElement(i,PLd - bidirectionalPathLossJitter));
				totalElements++;	//keep track of pathLoss size for reporting purposes
			}
		}
	}

	trace() << "Number of distinct space cells: " << numOfSpaceCells;
	trace() << "Each cell affects " <<
		(double)totalElements / numOfSpaceCells << " other cells on average";
	trace() << "The pathLoss array of lists was allocated in " <<
	    (double)(totalElements * elementSize) / 1048576 << " MBytes";
	// The larger this number, the slower your simulation. Consider increasing the cell size,
	// decreasing the field size, or if you only have static nodes, decreasing the number of nodes

	/*********************************************************************
	 * Allocate nodesAffectedByTransmitter even for static nodes.
	 * This makes the code more compact. We also have temporal variations
	 * so the nodes that are affected are not necessarily the same.
	 *********************************************************************/
	nodesAffectedByTransmitter = new list<int>[numOfNodes];
	if (nodesAffectedByTransmitter == NULL)
		opp_error("Could not allocate array nodesAffectedByTransmitter\n");

	/************************************************************
	 * If direct assignment of link qualities is given at the
	 * omnetpp.ini file we parse the input and update pathLoss.
	 * This is only for static nodes. (onlyStaticNodes==TRUE)
	 ************************************************************/
	parsePathLossMap();

	/* Create temporal model object from parameters file (if given) */
	if (strlen(temporalModelParametersFile) > 0) {
		temporalModel = new channelTemporalModel(temporalModelParametersFile, 2);
		temporalModelDefined = true;
	} else {
		temporalModelDefined = false;
	}

	declareHistogram("Fade depth distribution", -50, 15, 13);

	trace() << "Time for Wireless Channel module initialization: " <<
	    (double)(clock() - startTime) / CLOCKS_PER_SEC << "secs";

	// TODO: Aqui é o melhor sitio para ficar os updates ao path loss depedendo dos obstaculos
	// 1. Temos que inicializar uma matriz de numNodes * 8 (os pontos cardeais) a -1
	int numNodes = this->getAncestorPar("numNodes");
	int pathLossMatrix[numNodes][8];
	memset(pathLossMatrix, -1, sizeof pathLossMatrix);

	// 2. Vamos percorrer todos os obstaculos e fazer o update no pathLoss
	int numObstacles = this->getAncestorPar("numObstacles");
	int numHorizontalCells = this->getAncestorPar("horizontal_cells");
    int numVerticalCells = this->getAncestorPar("vertical_cells");

    int i, wallPosition;
    int adjecentNodesID[2];
    Wall *wall;

    cModule *sensorNetwork = this->getParentModule();

    ev << "[WirelessChannel::initialize] Existem #" << numObstacles << " obstaculos. Vamos ver como afectam o pathLoss" << endl;

    for (i = 0; i < numObstacles; i++)
    {
        // 2.2. Vamos buscar o modulo
        wall = check_and_cast <Wall*>(sensorNetwork->getSubmodule("obstacle", i));

        ev << "[WirelessChannel::initialize] Estamos no obstaculo #" << i << " que esta na cell #" << wall->betweenCells[0] << endl;

        // 2.3. Só valerá a pena actualizar o pathloss se a outra cell n estiver fora do field
        if (wall->betweenCells[1] != -1)
        {
            ev << "[WirelessChannel::initialize] Existe nó vizinho com ID #" << wall->betweenCells[1] << ". Vamos fazer o update do pathLoss o nó #" <<
                    wall->betweenCells[1] << " para o #" << wall->betweenCells[0] << endl;

            // 2.4. Atualizamos o path do betweenCells[1] para o betweenCells[0]
            this->updatePathLossElement(wall->betweenCells[1], wall->betweenCells[0], 150);
            wallPosition = (wall->wallPosition + 4) % 8;
            pathLossMatrix[wall->betweenCells[1]][wallPosition] = wall->betweenCells[0];
            ev << "[WirelessChannel::initialize] Meto na matrix pathLoss [" << wall->betweenCells[1] << "][" << wallPosition << "] a cell #"
                                << wall->betweenCells[0] << endl;

            // TODO: 2.4.1 Vamos ainda verificar se nós adjacentes têm wall na mesma direção. Se tiver temos que atualizar tbm o pathLoss
            // 2.4.1.2. Vamos agora descobrir qual é a posição da wall e set os nós adjacentes
            switch (wallPosition)
            {
                case 0:
                case 4:
                {
                    // É uma wall a Norte ou Sul por isso temos que ir verificar nos nós vizinhos Este e Oeste
                    if (((wall->betweenCells[1] + 1) % numHorizontalCells) == 0)
                    {
                        adjecentNodesID[0] = -1;
                    }
                    else
                    {
                        adjecentNodesID[0] = wall->betweenCells[1] + 1;
                    }

                    if ((wall->betweenCells[1] % numHorizontalCells) == 0)
                    {
                        adjecentNodesID[1] = -1;
                    }
                    else
                    {
                        adjecentNodesID[1] = wall->betweenCells[1] - 1;
                    }

                    ev << "[WirelessChannel::initialize] É uma wall a NORTE ou SUL por isso os nós adjacentes do nó #" << wall->betweenCells[1]
                     << " são a ESTE o #" << adjecentNodesID[0] << " e a OESTE o #" <<  adjecentNodesID[1] << endl;

                    if (adjecentNodesID[0] != -1)
                    {
                       if (pathLossMatrix[adjecentNodesID[0]][wallPosition] != -1)
                       {
                           if(wallPosition == 0) // Wall a NORTE
                           {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                       << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[1] << " e #"
                                       << pathLossMatrix[adjecentNodesID[0]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[adjecentNodesID[0]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[0]][wallPosition], wall->betweenCells[1], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                    << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[1]][wallPosition] << " e #"
                                    << adjecentNodesID[0] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][wallPosition], adjecentNodesID[0], 150);
                               this->updatePathLossElement(adjecentNodesID[0], pathLossMatrix[wall->betweenCells[1]][wallPosition], 150);
                           }
                           else // Wall a SUL
                           {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[1] << " e #"
                                      << pathLossMatrix[adjecentNodesID[0]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[adjecentNodesID[0]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[0]][wallPosition], wall->betweenCells[1], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                     << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[1]][wallPosition] << " e #"
                                     << adjecentNodesID[0] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][wallPosition], adjecentNodesID[0], 150);
                               this->updatePathLossElement(adjecentNodesID[0], pathLossMatrix[wall->betweenCells[1]][wallPosition], 150);
                           }
                       }
                    }

                    if (adjecentNodesID[1] != -1)
                    {
                        if (pathLossMatrix[adjecentNodesID[1]][wallPosition] != -1)
                        {
                            if(wallPosition == 2) // Wall a NORTE
                            {
                                ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                        << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[1] << " e #"
                                       << pathLossMatrix[adjecentNodesID[1]][wallPosition] << endl;

                                this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[adjecentNodesID[1]][wallPosition], 150);
                                this->updatePathLossElement(pathLossMatrix[adjecentNodesID[1]][wallPosition], wall->betweenCells[1], 150);

                                ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                         << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[1]][wallPosition] << " e #"
                                         << adjecentNodesID[1] << endl;

                                this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][wallPosition], adjecentNodesID[1], 150);
                                this->updatePathLossElement(adjecentNodesID[1], pathLossMatrix[wall->betweenCells[1]][wallPosition], 150);
                            }
                            else // Wall a SUL
                            {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[1] << " e #"
                                      << pathLossMatrix[adjecentNodesID[1]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[adjecentNodesID[1]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[1]][wallPosition], wall->betweenCells[1], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                    << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[1]][wallPosition] << " e #"
                                    << adjecentNodesID[1] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][wallPosition], adjecentNodesID[1], 150);
                               this->updatePathLossElement(adjecentNodesID[1], pathLossMatrix[wall->betweenCells[1]][wallPosition], 150);
                            }
                        }
                    }

                    break;
                }
                case 2:
                case 6:
                    // É uma wall a Norte ou Sul por isso temos que ir verificar nos nós vizinhos Este e Oeste
                    if (wall->betweenCells[1] - numHorizontalCells < 0)
                    {
                        adjecentNodesID[0] = -1;
                    }
                    else
                    {
                        adjecentNodesID[0] = wall->betweenCells[1] - numHorizontalCells;
                    }

                    if (wall->betweenCells[1] + numHorizontalCells >= (numHorizontalCells * numVerticalCells))
                    {
                        adjecentNodesID[1] = -1;
                    }
                    else
                    {
                        adjecentNodesID[1] = wall->betweenCells[1] + numHorizontalCells;
                    }


                    ev << "[WirelessChannel::initialize] É uma wall a ESTE ou OESTE por isso os nós adjacentes do nó #" << wall->betweenCells[1]
                     << " são a NORTE o #" << adjecentNodesID[0] << " e a SUL o #" <<  adjecentNodesID[1] << endl;

                    // 2.5.3. Por fim, temos que verificar se existem (!= -1) e se contêm tbm wall na mesma wallPosition. Se isto for verdade vamos aplicar o pathLoss
                    if (adjecentNodesID[0] != -1)
                    {
                       if (pathLossMatrix[adjecentNodesID[0]][wallPosition] != -1)
                       {
                           if(wallPosition == 2) // Wall a ESTE
                           {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                       << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[1] << " e #"
                                       << pathLossMatrix[adjecentNodesID[0]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[adjecentNodesID[0]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[0]][wallPosition], wall->betweenCells[1], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                       << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[1]][wallPosition] << " e #"
                                       << adjecentNodesID[0] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][wallPosition], adjecentNodesID[0], 150);
                               this->updatePathLossElement(adjecentNodesID[0], pathLossMatrix[wall->betweenCells[1]][wallPosition], 150);
                           }
                           else // Wall a OESTE
                           {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[1] << " e #"
                                      << pathLossMatrix[adjecentNodesID[0]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[adjecentNodesID[0]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[0]][wallPosition], wall->betweenCells[1], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[1]][wallPosition] << " e #"
                                      << adjecentNodesID[0] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][wallPosition], adjecentNodesID[0], 150);
                               this->updatePathLossElement(adjecentNodesID[0], pathLossMatrix[wall->betweenCells[1]][wallPosition], 150);
                           }
                       }
                    }

                    if (adjecentNodesID[1] != -1)
                    {
                        if (pathLossMatrix[adjecentNodesID[1]][wallPosition] != -1)
                        {
                            if(wallPosition == 2) // Wall a ESTE
                            {
                                ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                        << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[1] << " e #"
                                       << pathLossMatrix[adjecentNodesID[1]][wallPosition] << endl;

                                this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[adjecentNodesID[1]][wallPosition], 150);
                                this->updatePathLossElement(pathLossMatrix[adjecentNodesID[1]][wallPosition], wall->betweenCells[1], 150);

                                ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                     << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[1]][wallPosition] << " e #"
                                     << adjecentNodesID[1] << endl;

                                this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][wallPosition],adjecentNodesID[1], 150);
                                this->updatePathLossElement(adjecentNodesID[1], pathLossMatrix[wall->betweenCells[1]][wallPosition], 150);
                            }
                            else // Wall a OESTE
                            {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[1] << " e #"
                                      << pathLossMatrix[adjecentNodesID[1]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[adjecentNodesID[1]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[1]][wallPosition], wall->betweenCells[1], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                    << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[1]][wallPosition] << " e #"
                                    << adjecentNodesID[1] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][wallPosition],adjecentNodesID[1], 150);
                               this->updatePathLossElement(adjecentNodesID[1], pathLossMatrix[wall->betweenCells[1]][wallPosition], 150);
                            }
                        }
                    }

                    break;
            }

            // 2.4.2. Vamos verificar se o pontos cardeais secundarios n têm tbm de ser atualizados
            if (pathLossMatrix[wall->betweenCells[1]][(wallPosition + 2) % 8] != -1)
            {
                ev << "[WirelessChannel::initialize] Existe tbm uma wall na posição #" << (wallPosition + 2) % 8 << " da cell #" << wall->betweenCells[1] << endl;

                // Temos outra wall por isso temos que descobrir que nós atualizar no pathloss
                switch (wallPosition)
                {
                    case 0:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Norte e temos uma a Este. Vamos fazer o pathLoss entre #" << wall->betweenCells[1]
                           << "e #" << pathLossMatrix[wall->betweenCells[1]][0] + 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[wall->betweenCells[1]][0] + 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][0] + 1, wall->betweenCells[1], 150);
                        break;
                    }
                    case 2:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Este e temos uma a Sul. Vamos fazer o pathLoss entre #" << wall->betweenCells[1]
                           << "e #" << pathLossMatrix[wall->betweenCells[1]][4] + 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[wall->betweenCells[1]][4] + 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][4] + 1, wall->betweenCells[1], 150);
                        break;
                    }
                    case 4:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Sul e temos uma a Oeste. Vamos fazer o pathLoss entre #" << wall->betweenCells[1]
                           << "e #" << pathLossMatrix[wall->betweenCells[1]][4] - 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[wall->betweenCells[1]][4] - 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][4] - 1, wall->betweenCells[1], 150);
                        break;
                    }
                    case 6:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Oeste e temos uma a Norte. Vamos fazer o pathLoss entre #" << wall->betweenCells[1]
                           << "e #" << pathLossMatrix[wall->betweenCells[1]][0] - 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[wall->betweenCells[1]][0] - 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][0] - 1, wall->betweenCells[1], 150);
                        break;
                    }
                }

            }
            if (pathLossMatrix[wall->betweenCells[1]][(wallPosition + 6) % 8] != -1)
            {
                ev << "[WirelessChannel::initialize] Existe tbm uma wall na posição #" << (wallPosition + 6) % 8 << " da cell #" << wall->betweenCells[1] << endl;

                // Temos outra wall por isso temos que descobrir que nós atualizar no pathloss
                switch (wallPosition)
                {
                    case 0:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Norte e temos uma a Oeste. Vamos fazer o pathLoss entre #" << wall->betweenCells[1]
                           << " e #" << pathLossMatrix[wall->betweenCells[1]][0] - 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[wall->betweenCells[1]][0] - 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][0] - 1, wall->betweenCells[1], 150);
                        break;
                    }
                    case 2:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Este e temos uma a Norte. Vamos fazer o pathLoss entre #" << wall->betweenCells[1]
                           << " e #" << pathLossMatrix[wall->betweenCells[1]][0] + 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[wall->betweenCells[1]][0] + 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][0] + 1, wall->betweenCells[1], 150);
                        break;
                    }
                    case 4:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Sul e temos uma a Este. Vamos fazer o pathLoss entre #" << wall->betweenCells[1]
                           << " e #" << pathLossMatrix[wall->betweenCells[1]][4] + 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[wall->betweenCells[1]][4] + 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][4] + 1, wall->betweenCells[1], 150);
                        break;
                    }
                    case 6:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Oeste e temos uma a Sul. Vamos fazer o pathLoss entre #" << wall->betweenCells[1]
                           << " e #" << pathLossMatrix[wall->betweenCells[1]][4] - 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[1], pathLossMatrix[wall->betweenCells[1]][4] - 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[1]][4] - 1, wall->betweenCells[1], 150);
                        break;
                    }
                }
            }

            // 2.5. Atualizamos o path do betweenCells[0] para o betweenCells[1]
            ev << "[WirelessChannel::initialize] Vamos fazer o update no pathLoss p/ o nó #" << wall->betweenCells[0] << " para o #" << wall->betweenCells[1] << endl;
            this->updatePathLossElement(wall->betweenCells[0], wall->betweenCells[1], 150);
            wallPosition = wall->wallPosition;
            pathLossMatrix[wall->betweenCells[0]][wallPosition] = wall->betweenCells[1];
            ev << "[WirelessChannel::initialize] Meto na matrix pathLoss [" << wall->betweenCells[0] << "][" << wallPosition << "] a cell #"
                    << wall->betweenCells[1] << endl;

            // TODO: 2.5.1 Vamos ainda verificar se nós adjacentes têm wall na mesma direção. Se tiver temos que atualizar tbm o pathLoss
            // 2.5.1.1. Vamos agora descobrir qual é a posição da wall e set os nós adjacentes
            switch (wallPosition)
            {
                case 0:
                case 4:
                {
                    // É uma wall a Norte ou Sul por isso temos que ir verificar nos nós vizinhos Este e Oeste
                    if (((wall->betweenCells[0] + 1) % numHorizontalCells) == 0)
                    {
                        adjecentNodesID[0] = -1;
                    }
                    else
                    {
                        adjecentNodesID[0] = wall->betweenCells[0] + 1;
                    }

                    if ((wall->betweenCells[0] % numHorizontalCells) == 0)
                    {
                        adjecentNodesID[1] = -1;
                    }
                    else
                    {
                        adjecentNodesID[1] = wall->betweenCells[0] - 1;
                    }

                    ev << "[WirelessChannel::initialize] É uma wall a NORTE ou SUL por isso os nós adjacentes do nó #" << wall->betweenCells[0]
                     << " são a ESTE o #" << adjecentNodesID[0] << " e a OESTE o #" <<  adjecentNodesID[1] << endl;

                    if (adjecentNodesID[0] != -1)
                    {
                       if (pathLossMatrix[adjecentNodesID[0]][wallPosition] != -1)
                       {
                           if(wallPosition == 0) // Wall a NORTE
                           {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                       << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[0] << " e #"
                                       << pathLossMatrix[adjecentNodesID[0]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[adjecentNodesID[0]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[0]][wallPosition], wall->betweenCells[0], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                    << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[0]][wallPosition] << " e #"
                                    << adjecentNodesID[0] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][wallPosition], adjecentNodesID[0], 150);
                               this->updatePathLossElement(adjecentNodesID[0], pathLossMatrix[wall->betweenCells[0]][wallPosition], 150);
                           }
                           else // Wall a SUL
                           {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[0] << " e #"
                                      << pathLossMatrix[adjecentNodesID[0]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[adjecentNodesID[0]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[0]][wallPosition], wall->betweenCells[0], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                     << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[0]][wallPosition] << " e #"
                                     << adjecentNodesID[0] << endl;
                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][wallPosition], adjecentNodesID[0], 150);
                               this->updatePathLossElement(adjecentNodesID[0], pathLossMatrix[wall->betweenCells[0]][wallPosition], 150);
                           }
                       }
                    }

                    if (adjecentNodesID[1] != -1)
                    {
                        if (pathLossMatrix[adjecentNodesID[1]][wallPosition] != -1)
                        {
                            if(wallPosition == 2) // Wall a NORTE
                            {
                                ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                        << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[0] << " e #"
                                       << pathLossMatrix[adjecentNodesID[1]][wallPosition] << endl;

                                this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[adjecentNodesID[1]][wallPosition], 150);
                                this->updatePathLossElement(pathLossMatrix[adjecentNodesID[1]][wallPosition], wall->betweenCells[0], 150);

                                ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                         << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[0]][wallPosition] << " e #"
                                         << adjecentNodesID[1] << endl;

                                this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][wallPosition], adjecentNodesID[1], 150);
                                this->updatePathLossElement(adjecentNodesID[1], pathLossMatrix[wall->betweenCells[0]][wallPosition], 150);
                            }
                            else // Wall a SUL
                            {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[0] << " e #"
                                      << pathLossMatrix[adjecentNodesID[1]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[adjecentNodesID[1]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[1]][wallPosition], wall->betweenCells[0], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                    << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[0]][wallPosition] << " e #"
                                    << adjecentNodesID[1] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][wallPosition], adjecentNodesID[1], 150);
                               this->updatePathLossElement(adjecentNodesID[1], pathLossMatrix[wall->betweenCells[0]][wallPosition], 150);
                            }
                        }
                    }

                    break;
                }
                case 2:
                case 6:
                    // É uma wall a Norte ou Sul por isso temos que ir verificar nos nós vizinhos Este e Oeste
                    if (wall->betweenCells[0] - numHorizontalCells < 0)
                    {
                        adjecentNodesID[0] = -1;
                    }
                    else
                    {
                        adjecentNodesID[0] = wall->betweenCells[0] - numHorizontalCells;
                    }

                    if (wall->betweenCells[0] + numHorizontalCells >= (numHorizontalCells * numVerticalCells))
                    {
                        adjecentNodesID[1] = -1;
                    }
                    else
                    {
                        adjecentNodesID[1] = wall->betweenCells[0] + numHorizontalCells;
                    }


                    ev << "[WirelessChannel::initialize] É uma wall a ESTE ou OESTE por isso os nós adjacentes do nó #" << wall->betweenCells[0]
                     << " são a NORTE o #" << adjecentNodesID[0] << " e a SUL o #" <<  adjecentNodesID[1] << endl;

                    // 2.5.3. Por fim, temos que verificar se existem (!= -1) e se contêm tbm wall na mesma wallPosition. Se isto for verdade vamos aplicar o pathLoss
                    if (adjecentNodesID[0] != -1)
                    {
                       if (pathLossMatrix[adjecentNodesID[0]][wallPosition] != -1)
                       {
                           if(wallPosition == 2) // Wall a ESTE
                           {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                       << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[0] << " e #"
                                       << pathLossMatrix[adjecentNodesID[0]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[adjecentNodesID[0]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[0]][wallPosition], wall->betweenCells[0], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                       << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[0]][wallPosition] << " e #"
                                       << adjecentNodesID[0] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][wallPosition], adjecentNodesID[0], 150);
                               this->updatePathLossElement(adjecentNodesID[0], pathLossMatrix[wall->betweenCells[0]][wallPosition], 150);
                           }
                           else // Wall a OESTE
                           {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[0] << " e #"
                                      << pathLossMatrix[adjecentNodesID[0]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[adjecentNodesID[0]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[0]][wallPosition], wall->betweenCells[0], 150);


                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[0] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[0]][wallPosition] << " e #"
                                      << adjecentNodesID[0] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][wallPosition], adjecentNodesID[0], 150);
                               this->updatePathLossElement(adjecentNodesID[0], pathLossMatrix[wall->betweenCells[0]][wallPosition], 150);
                           }
                       }
                    }

                    if (adjecentNodesID[1] != -1)
                    {
                        if (pathLossMatrix[adjecentNodesID[1]][wallPosition] != -1)
                        {
                            if(wallPosition == 2) // Wall a ESTE
                            {
                                ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                        << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[0] << " e #"
                                       << pathLossMatrix[adjecentNodesID[1]][wallPosition] << endl;

                                this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[adjecentNodesID[1]][wallPosition], 150);
                                this->updatePathLossElement(pathLossMatrix[adjecentNodesID[1]][wallPosition], wall->betweenCells[0], 150);

                                ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                     << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[0]][wallPosition] << " e #"
                                     << adjecentNodesID[1] << endl;

                                this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][wallPosition], adjecentNodesID[1], 150);
                                this->updatePathLossElement(adjecentNodesID[1], pathLossMatrix[wall->betweenCells[0]][wallPosition], 150);
                            }
                            else // Wall a OESTE
                            {
                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                      << " pelo que temos de fazer o pathLoss entre #" <<  wall->betweenCells[0] << " e #"
                                      << pathLossMatrix[adjecentNodesID[1]][wallPosition] << endl;

                               this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[adjecentNodesID[1]][wallPosition], 150);
                               this->updatePathLossElement(pathLossMatrix[adjecentNodesID[1]][wallPosition], wall->betweenCells[0], 150);

                               ev << "[WirelessChannel::initialize] O nó #" << adjecentNodesID[1] << " contem wall a " << wallPosition
                                    << " pelo que temos de fazer o pathLoss entre #" <<  pathLossMatrix[wall->betweenCells[0]][wallPosition] << " e #"
                                    << adjecentNodesID[1] << endl;

                               this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][wallPosition], adjecentNodesID[1], 150);
                               this->updatePathLossElement(adjecentNodesID[1], pathLossMatrix[wall->betweenCells[0]][wallPosition], 150);
                            }
                        }
                    }

                    break;
            }

            // 2.5.2. Vamos verificar se o pontos cardeais secundarios n têm tbm de ser atualizados
            if (pathLossMatrix[wall->betweenCells[0]][(wallPosition + 2) % 8] != -1)
            {
                ev << "[WirelessChannel::initialize] Existe tbm uma wall na posição #" << (wallPosition + 2) % 8 << " da cell #" << wall->betweenCells[0] << endl;

                // Temos outra wall por isso temos que descobrir que nós atualizar no pathloss
                switch (wallPosition)
                {
                    case 0:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Norte e temos uma a Este. Vamos fazer o pathLoss entre #" << wall->betweenCells[0]
                           << " e #" << pathLossMatrix[wall->betweenCells[0]][0] + 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[wall->betweenCells[0]][0] + 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][0] + 1, wall->betweenCells[0], 150);
                        break;
                    }
                    case 2:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Este e temos uma a Sul. Vamos fazer o pathLoss entre #" << wall->betweenCells[0]
                           << " e #" << pathLossMatrix[wall->betweenCells[0]][4] + 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[wall->betweenCells[0]][4] + 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][4] + 1, wall->betweenCells[0], 150);
                        break;
                    }
                    case 4:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Sul e temos uma a Oeste. Vamos fazer o pathLoss entre #" << wall->betweenCells[0]
                           << " e #" << pathLossMatrix[wall->betweenCells[0]][4] - 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[wall->betweenCells[0]][4] - 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][4] - 1, wall->betweenCells[0], 150);
                        break;
                    }
                    case 6:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Oeste e temos uma a Norte. Vamos fazer o pathLoss entre #" << wall->betweenCells[0]
                           << " e #" << pathLossMatrix[wall->betweenCells[0]][0] - 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[wall->betweenCells[0]][0] - 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][0] - 1, wall->betweenCells[0], 150);
                        break;
                    }
                }
            }
            if (pathLossMatrix[wall->betweenCells[0]][(wallPosition + 6) % 8] != -1)
            {
                ev << "[WirelessChannel::initialize] Existe tbm uma wall na posição #" << (wallPosition + 6) % 8 << " da cell #" << wall->betweenCells[0] << endl;

                // Temos outra wall por isso temos que descobrir que nós atualizar no pathloss
                switch (wallPosition)
                {
                    case 0:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Norte e temos uma a Oeste. Vamos fazer o pathLoss entre #" << wall->betweenCells[0]
                           << " e #" << pathLossMatrix[wall->betweenCells[0]][0] - 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[wall->betweenCells[0]][0] - 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][0] - 1, wall->betweenCells[0], 150);
                        break;
                    }
                    case 2:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Este e temos uma a Norte. Vamos fazer o pathLoss entre #" << wall->betweenCells[0]
                           << " e #" << pathLossMatrix[wall->betweenCells[0]][0] + 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[wall->betweenCells[0]][0] + 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][0] + 1, wall->betweenCells[0], 150);
                        break;
                    }
                    case 4:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Sul e temos uma a Este. Vamos fazer o pathLoss entre #" << wall->betweenCells[0]
                           << " e #" << pathLossMatrix[wall->betweenCells[0]][4] + 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[wall->betweenCells[0]][4] + 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][4] + 1, wall->betweenCells[0], 150);
                        break;
                    }
                    case 6:
                    {
                        ev << "[WirelessChannel::initialize] Esta wall é a Oeste e temos uma a Sul. Vamos fazer o pathLoss entre #" << wall->betweenCells[0]
                           << " e #" << pathLossMatrix[wall->betweenCells[0]][4] - 1 << endl;
                        this->updatePathLossElement(wall->betweenCells[0], pathLossMatrix[wall->betweenCells[0]][4] - 1, 150);
                        this->updatePathLossElement(pathLossMatrix[wall->betweenCells[0]][4] - 1, wall->betweenCells[0], 150);
                        break;
                    }
                }
            }
        }
        else
        {
            ev << "[WirelessChannel::initialize] Não vale a pena atualizar o pathLoss porque o nó #" << wall->betweenCells[0] << " n tem vizinho." << endl;
        }

        // TODO: Temos de ver se os pontos cardeais secundários (NW, etc) têm tbm de ser atualizados
    }

    trace() << "Time for Wireless Channel module initialization: " <<
            (double)(clock() - startTime) / CLOCKS_PER_SEC << "secs";
}

/*****************************************************************************
 * This is where the main work is done by processing all the messages received
 *****************************************************************************/
void WirelessChannel::handleMessage(cMessage * msg)
{
	switch (msg->getKind()) {

	case WC_NODE_MOVEMENT:{

			WirelessChannelNodeMoveMessage *mobilityMsg =
				check_and_cast <WirelessChannelNodeMoveMessage*>(msg);
			int srcAddr = mobilityMsg->getNodeID();

	    /*****************************************************
	     * A node notified the wireless channel that it moved
	     * to a new space cell. Update the nodeLocation and
	     * based on the new cell calculation decide if the
	     * cellOccupation array needs to be updated.
	     *****************************************************/

			if (onlyStaticNodes)
				opp_error("Error: Rerceived WS_NODE_MOVEMENT msg, while onlyStaticNodes is TRUE");

			int oldCell = nodeLocation[srcAddr].cell;
			nodeLocation[srcAddr].x = mobilityMsg->getX();
			nodeLocation[srcAddr].y = mobilityMsg->getY();
			nodeLocation[srcAddr].z = mobilityMsg->getZ();
			nodeLocation[srcAddr].phi = mobilityMsg->getPhi();
			nodeLocation[srcAddr].theta = mobilityMsg->getTheta();
			if ((nodeLocation[srcAddr].x < 0.0) ||
				(nodeLocation[srcAddr].y < 0.0) ||
				(nodeLocation[srcAddr].z < 0.0))
					opp_error("Wireless channel received faulty WC_NODE_MOVEMENT msg. We cannot have negative node coordinates");

			int xIndex = (int)floor(nodeLocation[srcAddr].x / xFieldSize * numOfXCells);
			if (((xIndex - 1) * xCellSize) >= nodeLocation[srcAddr].x)
				xIndex--;
			else if (xIndex >= numOfXCells) {
				xIndex = numOfXCells - 1;	// the maximum possible x index
				if (nodeLocation[srcAddr].x > xFieldSize)
					debug() << "WARNING at WC_NODE_MOVEMENT: node position out of bounds in X dimension!\n";
			}

			int yIndex = (int)floor(nodeLocation[srcAddr].y / yFieldSize * numOfYCells);
			if (((yIndex - 1) * yCellSize) >= nodeLocation[srcAddr].y)
				yIndex--;
			else if (yIndex >= numOfYCells) {
				yIndex = numOfYCells - 1;	// the maximum possible y index
				if (nodeLocation[srcAddr].y > yFieldSize)
					debug() << "WARNING at WC_NODE_MOVEMENT: node position out of bounds in Y dimension!\n";
			}

			int zIndex = (int)floor(nodeLocation[srcAddr].z / zFieldSize * numOfZCells);
			if (((zIndex - 1) * zCellSize) >= nodeLocation[srcAddr].z)
				zIndex--;
			else if (zIndex >= numOfZCells) {
				zIndex = numOfZCells - 1;	// the maximum possible z index
				if (nodeLocation[srcAddr].z > zFieldSize)
					debug() << "WARNING at WC_NODE_MOVEMENT: node position out of bounds in Z dimension!\n";
			}

			int newCell = zIndex * zIndexIncrement + yIndex * yIndexIncrement + xIndex * xIndexIncrement;
			if (newCell != oldCell) {
				cellOccupation[oldCell].remove(srcAddr);
				cellOccupation[newCell].push_front(srcAddr);
				nodeLocation[srcAddr].cell = newCell;
			}

			break;
		}

	case WC_SIGNAL_START:
	{
	    ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] Entramos aqui" << endl;

        WirelessChannelSignalBegin *signalMsg =
            check_and_cast <WirelessChannelSignalBegin*>(msg);
        int srcAddr = signalMsg->getNodeID();
        int receptioncount = 0;

        /* Find the cell that the transmitting node resides */
        int cellTx = nodeLocation[srcAddr].cell;

        ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] É o start começado pelo nó #" << srcAddr << " que está na cell #" << cellTx <<endl;

        /* Iterate through the list of cells that are affected
         * by cellTx and check if there are nodes there.
         * Update the nodesAffectedByTransmitter array
         */
        ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] Vamos verificar que celulas devem receber o broadcast" << endl;
        list < PathLossElement * >::iterator it1;
        for (it1 = pathLoss[cellTx].begin(); it1 != pathLoss[cellTx].end(); it1++)
        {
            ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] A verificar a cell #" << (*it1)->cellID << endl;

            /* If no nodes exist in this cell, move on. */
            if (cellOccupation[(*it1)->cellID].empty())
            {
                ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] Não existe nó nesta cell" << endl;
                continue;
            }

            ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] Existe nó nesta cell" << endl;

            /* Otherwise there are some nodes in that cell.
             * Calculate the signal received by these nodes
             * It is exactly the same for all of them.
             * The signal may be variable in time.
             */
            float currentSignalReceived = signalMsg->getPower_dBm() - (*it1)->avgPathLoss;
            if (temporalModelDefined) {
                simtime_t timePassed_msec = (simTime() - (*it1)->lastObservationTime) * 1000;
                simtime_t timeProcessed_msec =
                        temporalModel->runTemporalModel(SIMTIME_DBL(timePassed_msec),
                        &((*it1)->lastObservedDiffFromAvgPathLoss));
                currentSignalReceived += (*it1)->lastObservedDiffFromAvgPathLoss;
                collectHistogram("Fade depth distribution",
                         (*it1)->lastObservedDiffFromAvgPathLoss);
                /* Update the observation time */
                (*it1)->lastObservationTime = simTime() -
                        (timePassed_msec - timeProcessed_msec) / 1000;
            }

            /* If the resulting current signal received is not strong enough,
             * to be delivered to the radio module, continue to the next cell.
             */
            if (currentSignalReceived < signalDeliveryThreshold){
                ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] O broadcast não chega a este nó" << endl;
                continue;
            }

            /* Else go through all the nodes of that cell.
             * Iterator it2 returns node IDs.
             */
            ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] Vamos agora percorrer todos os nós da cell #"  << (*it1)->cellID << " (deve ser só 1)" << endl;
            list < int >::iterator it2;
            for (it2 = cellOccupation[(*it1)->cellID].begin();
                    it2 != cellOccupation[(*it1)->cellID].end(); it2++) {
                if (*it2 == srcAddr)
                {
                    ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] O nó iterado é o mesmo que mandou o broadcast. Não enviamos" << endl;
                    continue;
                }
                // TODO: Precisamos ainda de perceber se não há obstaculos nesta celula

                receptioncount++;
                WirelessChannelSignalBegin *signalMsgCopy = signalMsg->dup();
                signalMsgCopy->setPower_dBm(currentSignalReceived);
                send(signalMsgCopy, "toNode", *it2);
                nodesAffectedByTransmitter[srcAddr].push_front(*it2);

                ev << "[WirelessChannel::handleMessage::WC_SIGNAL_START] Broadcast para o nó #" << *it2 << endl;
            }	//for it2
        }	//for it1

        if (receptioncount > 0)
        {
            trace() << "signal from node[" << srcAddr << "] reached " <<
                    receptioncount << " other nodes";
        }

        break;
	}

	case WC_SIGNAL_END:{
			WirelessChannelSignalEnd *signalMsg =
			    check_and_cast <WirelessChannelSignalEnd*>(msg);
			int srcAddr = signalMsg->getNodeID();

			/* Go through the list of nodes that were affected
			 *  by this transmission. *it1 holds the node ID
			 */
			list <int>::iterator it1;
			for (it1 = nodesAffectedByTransmitter[srcAddr].begin();
					it1 != nodesAffectedByTransmitter[srcAddr].end(); it1++) {
				WirelessChannelSignalEnd *signalMsgCopy = signalMsg->dup();
				send(signalMsgCopy, "toNode", *it1);
			}	//for it1

			/* Now that we are done processing the msg we delete the whole list
			 * nodesAffectedByTransmitter[srcAddr], since srcAddr in not TXing anymore.
			 */
			nodesAffectedByTransmitter[srcAddr].clear();
			break;
		}

	default:{
			opp_error("ERROR: Wireless Channel received unknown message kind=%i", msg->getKind());
			break;
		}
	}
	delete msg;
}

void WirelessChannel::finishSpecific()
{

	/*****************************************************
	 * Delete dynamically allocated arrays. Some allocate
	 * lists of objects so they need an extra nested loop
	 * to properly deallocate all these objects
	 *****************************************************/

	/* delete pathLoss */
	for (int i = 0; i < numOfSpaceCells; i++) {
		list <PathLossElement*>::iterator it1;
		for (it1 = pathLoss[i].begin(); it1 != pathLoss[i].end(); it1++) {
			delete(*it1);	// deallocate the memory occupied by the object
		}
	}
	delete[]pathLoss;	// the delete[] operator releases memory allocated with new []

	/* delete nodesAffectedByTransmitter */
	delete[]nodesAffectedByTransmitter;	// the delete[] operator releases memory allocated with new []

	/* delete cellOccupation */
	delete[]cellOccupation;	// the delete[] operator releases memory allocated with new []

	/* delete nodeLocation */
	delete[]nodeLocation;

	if (temporalModelDefined)
		delete temporalModel;

	//close the output stream that CASTALIA_DEBUG is writing to
	DebugInfoWriter::closeStream();
}

void WirelessChannel::readIniFileParameters(void)
{
	DebugInfoWriter::setDebugFileName(
		getParentModule()->par("debugInfoFileName").stringValue());

	onlyStaticNodes = par("onlyStaticNodes");
	pathLossExponent = par("pathLossExponent");
	sigma = par("sigma");
	bidirectionalSigma = par("bidirectionalSigma");
	PLd0 = par("PLd0");
	d0 = par("d0");

	pathLossMapFile = par("pathLossMapFile");
	temporalModelParametersFile = par("temporalModelParametersFile");
	signalDeliveryThreshold = par("signalDeliveryThreshold");

	numOfNodes = getParentModule()->par("numNodes");
	xFieldSize = getParentModule()->par("field_x");
	yFieldSize = getParentModule()->par("field_y");
	zFieldSize = getParentModule()->par("field_z");
	xCellSize = par("xCellSize");
	yCellSize = par("yCellSize");
	zCellSize = par("zCellSize");

	maxTxPower = 0.0;

}				// readIniParameters

/* Parsing of custom pathloss map from a file,
 * filename given by the parameter pathLossMapFile
 */
void WirelessChannel::parsePathLossMap(void)
{
	if (strlen(pathLossMapFile) == 0)
		return;
	ifstream f(pathLossMapFile);
	if (!f.is_open())
		opp_error("\n[Wireless Channel]:\n Error reading from pathLossMapFile %s\n", pathLossMapFile);

	string s;
	const char *ct;
	int source, destination;
	float pathloss_db;

	/* each line in a file is in the same format:
	 * Transmitter_id>Receiver_id:PathLoss_dB, ... ,Receiver_id:PathLoss_dB
	 * based on these values we will update the pathloss array
	 * (using updatePathLossElement function)
	 */
	while (getline(f, s)) {
		ct = s.c_str();	//ct points to the beginning of a line
		while (ct[0] && (ct[0] == ' ' || ct[0] == '\t'))
			ct++;
		if (!ct[0] || ct[0] == '#')
			continue;	// skip comments
		if (parseInt(ct, &source))
			opp_error("\n[Wireless Channel]:\n Bad syntax in pathLossMapFile, expecting source identifier\n");
		while (ct[0] && ct[0] != '>')
			ct++;	//skip untill '>' character
		if (!ct[0])
			opp_error("\n[Wireless Channel]:\n Bad syntax in pathLossMapFile, expecting comma separated list of values\n");
		cStringTokenizer t(++ct, ",");	//divide the rest of the strig with comma delimiter
		while ((ct = t.nextToken())) {
			if (parseInt(ct, &destination))
				opp_error("\n[Wireless Channel]:\n Bad syntax in pathLossMapFile, expecting target identifier\n");
			while (ct[0] && ct[0] != ':')
				ct++;	//skip untill ':' character
			if (parseFloat(++ct, &pathloss_db))
				opp_error("\n[Wireless Channel]:\n Bad syntax in pathLossMapFile, expecting dB value for path loss\n");
			updatePathLossElement(source, destination, pathloss_db);
		}
	}
}

//This function will update a pathloss element for given source and destination cells with a given value of pathloss
//If this pair is already defined in pathloss array, the old value is replaced, otherwise a new pathloss element is created
void WirelessChannel::updatePathLossElement(int src, int dst, float pathloss_db)
{
	if (src >= numOfSpaceCells || dst >= numOfSpaceCells) return;
	list <PathLossElement*>::iterator it1;
	for (it1 = pathLoss[src].begin(); it1 != pathLoss[src].end(); it1++) {
		if ((*it1)->cellID == dst) {
			(*it1)->avgPathLoss = pathloss_db;
			return;
		}
	}
	pathLoss[src].push_front(new PathLossElement(dst, pathloss_db));
}

//wrapper function for atoi(...) call. returns 1 on error, 0 on success
int WirelessChannel::parseInt(const char *c, int *dst)
{
	while (c[0] && (c[0] == ' ' || c[0] == '\t'))
		c++;
	if (!c[0] || c[0] < '0' || c[0] > '9')
		return 1;
	*dst = atoi(c);
	return 0;
}

//wrapper function for strtof(...) call. returns 1 on error, 0 on success
int WirelessChannel::parseFloat(const char *c, float *dst)
{
	char *tmp;
	*dst = strtof(c, &tmp);
	if (c == tmp)
		return 1;
	return 0;
}
