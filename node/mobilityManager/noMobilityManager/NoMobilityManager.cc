/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Yuriy Tselishchev                                            *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *  
 ****************************************************************************/

#include "NoMobilityManager.h"
#include "PersonsPhysicalProcess.h"

Define_Module(NoMobilityManager);

void NoMobilityManager::initialize()
{
    VirtualMobilityManager::initialize();

    // 2nd - Get this node index
    cModule *fatherNode = this->getParentModule();
    int myIndex = fatherNode->getIndex();

    ev << "Olha aqui o index: " << myIndex << endl;

    // See where sensor node must stay in field
    // 1st - We must retrieve the field dimensions and cell dimension from parent
    cModule *grandFatherNode = this->getParentModule()->getParentModule();
    int numHorizontalCells = grandFatherNode->par("horizontal_cells");
    int numVerticalCells = grandFatherNode->par("vertical_cells");
    int cellSize = grandFatherNode->par("cell_size");
    int numCellsInGui = numHorizontalCells * numVerticalCells;

    ev << "Olha aqui os dados: " << numHorizontalCells << " | " << numVerticalCells << " | " << cellSize << " | " << numCellsInGui <<  endl;

    if (myIndex >= 0 && myIndex < numCellsInGui)
    {
        // We must discover where the sensor must be draw
        //// 1st - What line it should be draw
        int lineToDraw = myIndex / numHorizontalCells;
        nodeLocation.y = (lineToDraw * cellSize) + (cellSize / 2);
        fatherNode->par("yCoor") = nodeLocation.y;

        //// 2nd - What line it should be draw
        int collumnToDraw = myIndex % numHorizontalCells;
        nodeLocation.x = (collumnToDraw * cellSize) + (cellSize / 2);
        fatherNode->par("xCoor") = nodeLocation.x;

        ev << "Eu (Sensor Node #" << myIndex << " ) inicializei-me na posição (" << nodeLocation.x << ", " << nodeLocation.y << ")" << endl;

        char s[10];
        cDisplayString &nodeDS = fatherNode->getDisplayString();

        // Set x
        sprintf(s,"%f",nodeLocation.x);
        nodeDS.setTagArg("p",0,s);

        // Set y
        sprintf(s,"%f",nodeLocation.y);
        nodeDS.setTagArg("p",1,s);
    }
    // If node isn't between 0 and num of cells in GUI - 1 then can't be draw
    else
    {}

    // Regist in Statists Module
    cModule *physicalProcess = grandFatherNode->getModuleByPath("SensorNetwork.physicalProcess[0]");
    PersonsPhysicalProcess *ppp = check_and_cast<PersonsPhysicalProcess *>(physicalProcess);

    ppp->registrySensorNodes(this->getParentModule());
}

void NoMobilityManager::handleMessage(cMessage * msg)
{
	int msgKind = msg->getKind();
	trace() << "Unexpected message: " << msgKind;
	delete msg;
	msg = NULL;
}
