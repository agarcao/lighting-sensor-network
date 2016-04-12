#include "WsnLogic.h"

Define_Module(WsnLogic);

void WsnLogic::startup()
{
    maxSampleInterval = ((double)par("maxSampleInterval")) / 1000.0;
    minSampleInterval = ((double)par("minSampleInterval")) / 1000.0;
    currSentSampleSN = 0;
    randomBackoffIntervalFraction = genk_dblrand(0);
    sentOnce = false;

    // Set node ID
    this->self = this->getParentModule()->getIndex();

    this->timeToDiminishLightIntensity = par("timeToDiminishLightIntensity");

    this->coneLightingIsActive = par("coneLightingIsActive");
    this->radiousLighting = par("radiousLighting");

    this->movementDirection = -1;

    // Set o nós vizinhos
    const char *neighborsNodesIDsString = par("neighborsNodesIDs");
    int i = 0;

    cStringTokenizer tokenizer(neighborsNodesIDsString);
    while (tokenizer.hasMoreTokens())
    {
        this->neighborsNodesIds[i] = atoi(tokenizer.nextToken());
        i++;
    }

    //setTimer(REQUEST_SAMPLE, maxSampleInterval * randomBackoffIntervalFraction);
}

void WsnLogic::timerFiredCallback(int index)
{
    switch (index)
    {
        case WsnLogicTimers::DIMINISH_LIGHT:
        {
            handleSelfEvent();
            break;
        }
    }
}

/**
 * Função que é corrida quando recebemos um broadcast de outro nó da rede
 * */
void WsnLogic::fromNetworkLayer(ApplicationPacket * genericPacket,
         const char *source, double rssi, double lqi)
{
    /** Recebemos um broadcast */
    ev << "[Node #" << this->getParentModule()->getIndex() << "::WsnLogic::fromNetworkLayer] Recebemos um broadcast de outro nó da rede" << endl;
    WsnLogicDataPacket *rcvPacket = check_and_cast<WsnLogicDataPacket*>(genericPacket);
    WsnLogicData theData = rcvPacket->getExtraData();

    /* Em 1º lugar - Temos de ver se a mensagem broadcast for enviada/tem origem por este nó, ou, caso exista uma lista de destinátários ver se fazemos parte.
         Se nenhuma desta duas coisas forem verdade, não tratamos a mensagem. */
    if (this->self == theData.originNodeID || this->self == theData.senderNodeID)
    {
        ev << "[Node #" << this->getParentModule()->getIndex() << "::WsnLogic::fromNetworkLayer] Descartamos esta mensagem porque foi mandada ou tem origem neste nó" << endl;
        return;
    }

    // Em 3º lugar - O que vamos fazer a partir daqui dependerá do tipo de mensagem que foi recebido, por isso
    switch (theData.type)
    {
        case WSNLogicMessageTypes::ONLY_LIGHT_RADIOUS:
        {
            /** Neste caso recebemos uma broadcast onde o objectivo é só alargar o raio de iluminação */
            this->turnOnTheLight();

            int hopCounter = theData.hop + 1;   // O +1 é porque é o numero de hops que vai chegar no próximo nó
            if (hopCounter <= this->radiousLighting)
            {
                ev << "[Sensor Node #" << getParentModule()->getIndex() << "::WsnLogic::fromNetworkLayer] We are going broadcast the msg receive because hopCounter = "<< hopCounter << endl;
                WsnLogicData tmpData;
                tmpData.type = theData.type;
                tmpData.originNodeID = theData.originNodeID;
                tmpData.senderNodeID = (unsigned short)this->self;
                tmpData.hop = hopCounter;

                WsnLogicDataPacket *tmpPacket = rcvPacket->dup();
                tmpPacket->setExtraData(tmpData);

                toNetworkLayer(tmpPacket, BROADCAST_NETWORK_ADDRESS);
                this->sentOnce = true;

                ev << "[Sensor Node #" << getParentModule()->getIndex() << "::WsnLogic::fromNetworkLayer] End broadcasting the broadMsg" << endl;
            }
            else
            {
                ev << "[Sensor Node #" << getParentModule()->getIndex() << "::WsnLogic::fromNetworkLayer] We don't broadcast the broadcast msg because hop is 0" << endl;
            }
            break;
        }
        case WSNLogicMessageTypes::ONLY_LIGHT_CONE:
        {
            /** Neste caso recebemos uma broadcast onde o objectivo é criar o cone de ilumninação */
            // 1º - Verifico se sou vizinho ou não do nó que mandou a msg
            if (theData.hop == 1)
            {
                /** Sou vizinho por isso tenho sempre que guardar a direção do movimento, e caso a msg's DEST for para mim acender e reencaminhar o broadcast */
                ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Sou vizinho, tenho que guardar a direcção do movimento" << endl;

                //// 1º - Vou guardar a direcção do movimento
                this->movementDirection = this->getMovementDirectionFromSenderNodeID(theData.senderNodeID);
                ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Direcção do movimento a partir do senderNode #" << theData.senderNodeID << "é #" << this->movementDirection << endl;

                //// 2º - Se a msg for para mim, acendo e reencaminho broadcast
                if (this->self == theData.destinationNodesID)
                {
                    ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Sou vizinho e a msg é para mim = Vou acender e reencaminhar a msg" << endl;
                    // A mensagem é para mim por isso devo acender e manda a msg na mesma direção
                    this->turnOnTheLight(); // Chamo o metodo para acender a luz

                    // Temos de ver qual deve ser o destino da próxima msg
                    int dest = this->neighborsNodesIds[this->movementDirection];

                    if (dest != -1) // quer dizer que ainda tem vizinhos na direcção que queremos
                    {
                        ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Ainda tenho vizinho #" << dest << " na direcção #" << this->movementDirection << endl;
                        WsnLogicData tmpData;
                        tmpData.type = theData.type;
                        tmpData.originNodeID = theData.originNodeID;
                        tmpData.senderNodeID = (unsigned short)this->self;
                        tmpData.destinationNodesID = dest;
                        tmpData.hop = theData.hop + 1;

                        WsnLogicDataPacket *tmpPacket = rcvPacket->dup();
                        tmpPacket->setExtraData(tmpData);

                        toNetworkLayer(tmpPacket, BROADCAST_NETWORK_ADDRESS);
                        this->sentOnce = true;
                    }
                    else
                    {
                        ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Já não tenho vizinho na direcção #" << this->movementDirection << endl;
                    }
                }
                else // Msg não é para mim
                {
                    ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Sou vizinho mas a msg n é para mim = Não acendo nem reencaminho" << endl;
                }
            }
            else // Não sou vizinho por isso só faço algo se o campo DEST do BROAD for o meu id
            {
                if (this->self == theData.destinationNodesID)
                {
                    ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Não sou vizinho mas a msg é para mim. Vou acender e recaminhar" << endl;
                    // A mensagem é para mim por isso devo acender e manda a msg na mesma direção
                    this->turnOnTheLight(); // Chamo o metodo para acender a luz

                    // Temos de ver qual deve ser o destino da próxima msg
                    int movementDirection = this->getMovementDirectionFromSenderNodeID(theData.senderNodeID);
                    int dest = this->neighborsNodesIds[movementDirection];

                    if (dest != -1) // quer dizer que ainda tem vizinhos na direcção que queremos
                    {
                        ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Ainda tenho vizinho #" << dest << " na direcção #" << this->movementDirection << endl;
                        WsnLogicData tmpData;
                        tmpData.type = theData.type;
                        tmpData.originNodeID = theData.originNodeID;
                        tmpData.senderNodeID = (unsigned short)this->self;
                        tmpData.destinationNodesID = dest;
                        tmpData.hop = theData.hop + 1;

                        WsnLogicDataPacket *tmpPacket = rcvPacket->dup();
                        tmpPacket->setExtraData(tmpData);

                        toNetworkLayer(tmpPacket, BROADCAST_NETWORK_ADDRESS);
                        this->sentOnce = true;
                    }
                    else
                    {
                        ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Já não tenho vizinho na direcção #" << this->movementDirection << endl;
                    }
                }
                else // Não sou vizinho e a msg n é para mim = descarto
                {
                    ev << "[Sensor Node #" << this->self << "::WsnLogic::fromNetworkLayer] Not neighbor and msg's DEST is not for me = discard the msg" << endl;
                }
            }
            break;
        }
        case WSNLogicMessageTypes::LIGHT_RADIOUS_CONE:
        {
            break;
        }
        default:
        {
            opp_error("Broadcast came without any know type");
        }
    }

    ev << "[Sensor Node #" << getParentModule()->getIndex() << "::WsnLogic::fromNetworkLayer] Exiting handleSensorReading function" << endl;
}

void WsnLogic::handleSensorReading(SensorReadingMessage * rcvReading)
{
    // TODO: Receive SensorReadingMsg and do things
    ev << "[Sensor Node #" << getParentModule()->getIndex() << "::Application Module::WsnLogic::handleSensorReading] Enter handleSensorReading function" << endl;

    // 1st - We must say to the resource module to set the light up
    //// Construct the msg
    ev << "[Sensor Node #" << getParentModule()->getIndex() << "::Application Module::WsnLogic::handleSensorReading] We must send msg to Resource Module to set the light up" << endl;

    ostringstream s;
    s << "increaseLightSensor#" << getParentModule()->getIndex();
    string msg = s.str();
    ResourceManagerMessage *resourceManagerMsg = new ResourceManagerMessage(msg.c_str(), RESOURCE_MANAGER_LIGHT);
    resourceManagerMsg->setIncreaseLightIntensity(true);

    //// Send the msg to Resource
    send(resourceManagerMsg, "toResourceManager");
    ev << "[Sensor Node #" << getParentModule()->getIndex() << "::Application Module::WsnLogic::handleSensorReading] Msg to Resource Module sent" << endl;


    // 2nd - We must now create a self event to decrease the light intensity
    //// Cancel and Delete the self event (this way we dont need to check if event is already create or not
    ev << "[Sensor Node #" << getParentModule()->getIndex() << "::Application Module::WsnLogic::handleSensorReading] Create a new self event to diminish the light intensity" << endl;

    //// Set new timer
    setTimer(WsnLogicTimers::DIMINISH_LIGHT, this->timeToDiminishLightIntensity);

    ev << "[Sensor Node #" << getParentModule()->getIndex() << "::Application Module::WsnLogic::handleSensorReading] Self event to diminish the light intensity schedule" << endl;


    /** We must see if we must broadcast the event of detecting persons. We only do that if the 'coneLightingIsActive'
     * or if the 'radiousLighting' is more than 0
     **/
    if (this->coneLightingIsActive || this->radiousLighting > 0)
    {
        // 3rd - We need now to broadcast a msg to the neighbor nodes
        WsnLogicData tmpData;
        tmpData.originNodeID = (unsigned short)this->self;
        tmpData.senderNodeID = (unsigned short)this->self;
        tmpData.destinationNodesID = -1;
        tmpData.hop = 1;                                    // in the beginning the msg still don't have any hop in the network

        ostringstream s1;

        //// Escolher o tipo de mensagem
        if (this->coneLightingIsActive && this->radiousLighting > 0)
        {
            tmpData.type = WSNLogicMessageTypes::LIGHT_RADIOUS_CONE;
            s1 << "Broadcast msg from node #" << this->self << " with type LIGHT_RADIOUS_CONE";
        }
        else if (this->coneLightingIsActive)
        {
            tmpData.type = WSNLogicMessageTypes::ONLY_LIGHT_CONE;
            s1 << "Broadcast msg from node #" << this->self << " with type ONLY_LIGHT_CONE";
            // Tenho que ver se este nó tem alguma direção já previamente "setted"
            if(this->movementDirection != -1)
            {
                // Temos de meter o nó destino com o nº do nó vizinho daquela direção
                tmpData.destinationNodesID = this->neighborsNodesIds.find(this->movementDirection)->second;
                ev << "[Sensor Node #" << this->self << "::Application Module::WsnLogic::handleSensorReading] A msg tem com DEST #" << tmpData.destinationNodesID << endl;
            }
        }
        else if (this->radiousLighting > 0)
        {
            tmpData.type = WSNLogicMessageTypes::ONLY_LIGHT_RADIOUS;
            s1 << "Broadcast msg from node #" << this->self << " with type ONLY_LIGHT_RADIOUS";
        }
        else {
            ev << "[Sensor Node #" << this->self << "::Application Module::WsnLogic::handleSensorReading] Don't find any type" << endl;
        }

        msg = s1.str();

        WsnLogicDataPacket *packet2Net = new WsnLogicDataPacket(msg.c_str(), APPLICATION_PACKET);
        packet2Net->setExtraData(tmpData);
        packet2Net->setSequenceNumber(this->currSentSampleSN);
        this->currSentSampleSN++;

        toNetworkLayer(packet2Net, BROADCAST_NETWORK_ADDRESS);
        this->sentOnce = true;


        ev << "[Sensor Node #" << getParentModule()->getIndex() << "::Application Module::WsnLogic::handleSensorReading] Exiting handleSensorReading function" << endl;
        // int sensIndex =  rcvReading->getSensorIndex();
        // string sensType(rcvReading->getSensorType());
        //double sensValue = rcvReading->getSensedValue();

        // schedule the TX of the value
        //trace() << "Sensed = " << sensValue;

        //WsnLogicData tmpData;
        //tmpData.nodeID = (unsigned short)self;
        //tmpData.locX = mobilityModule->getLocation().x;
        //tmpData.locY = mobilityModule->getLocation().y;

       // WsnLogicDataPacket *packet2Net =
            //new WsnLogicDataPacket("Value reporting pck", APPLICATION_PACKET);
       // packet2Net->setExtraData(tmpData);
        //packet2Net->setData(sensValue);
        //packet2Net->setSequenceNumber(currSentSampleSN);
        //currSentSampleSN++;

        //toNetworkLayer(packet2Net, SINK_NETWORK_ADDRESS);
        //sentOnce = true;
    }
}

/*
 * Function to handle the self event
 */
void WsnLogic::handleSelfEvent()
{
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] Receive a diminishLightIntensityEvent. Let send info to Resource Module" << endl;

    // Send msg to Resource Module
    ostringstream s;
    s << "decreaseLightSensor#" << getParentModule()->getIndex();
    string msg = s.str();
    ResourceManagerMessage *resourceManagerMsg = new ResourceManagerMessage(msg.c_str(), RESOURCE_MANAGER_LIGHT);
    send(resourceManagerMsg, "toResourceManager");

    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] Msg to diminish the light intensity send to Resource Module" << endl;
}

void WsnLogic::turnOnTheLight()
{
    // Em 2º lugar - temos de acender a luz (caso ele ainda n tiver acesa) e renovar o timer que apaga a luz
    ev << "[Sensor Node #" << this->self << "::WsnLogic::turnOnTheLight] We must send msg to Resource Module to set the light up" << endl;

    //// Mandamos msg para o modulo 'resources' onde está a logica da lampada
    ostringstream s;
    s << "increaseLightSensor#" << this->self;
    string msg = s.str();
    ResourceManagerMessage *resourceManagerMsg = new ResourceManagerMessage(msg.c_str(), RESOURCE_MANAGER_LIGHT);
    resourceManagerMsg->setIncreaseLightIntensity(true);
    send(resourceManagerMsg, "toResourceManager");

    //// Cancelamos e eliminamos o self event e criamos um novo para não haver merdas
    ev << "[Sensor Node #" << this->self << "::WsnLogic::turnOnTheLight] Cancel the diminish light event" << endl;
    cancelTimer(WsnLogicTimers::DIMINISH_LIGHT);
    ev << "[Sensor Node #" << this->self << "::WsnLogic::turnOnTheLight] Create a new self event to diminish the light intensity" << endl;
    setTimer(WsnLogicTimers::DIMINISH_LIGHT, this->timeToDiminishLightIntensity);
    ev << "[Sensor Node #" << this->self << "::WsnLogic::turnOnTheLight] Self event to diminish the light intensity schedule" << endl;
}

int WsnLogic::getMovementDirectionFromSenderNodeID(int senderNodeID)
{
    // 1º - Check qual é a key do _senderNodeID_ no mapa de vizinhos
    int key = -1;
    ev << "[Sensor Node #" << this->self << "::WsnLogic::getDestFromSenderNodeID] Vamos verificar qual é a key do nó #" << senderNodeID << endl;

    for (auto& neighbor: this->neighborsNodesIds) {
        if (senderNodeID == neighbor.second)
        {
            ev << "[Sensor Node #" << this->self << "::WsnLogic::getDestFromSenderNodeID] Key do nó #" << senderNodeID << " é #" << neighbor.first << endl;
            key = neighbor.first;
            break;
        }
    }

    // 2º - Fazemos o mapeamento
    int directionKey = -1;
    if (key != -1)
    {
        switch (key)
        {
            case PersonMovementDirections::NORTH:
            {
                directionKey = PersonMovementDirections::SOUTH;
                break;
            }
            case PersonMovementDirections::NORTHEAST:
            {
                directionKey = PersonMovementDirections::SOUTHWEST;
                break;
            }
            case PersonMovementDirections::EAST:
            {
                directionKey = PersonMovementDirections::WEST;
                break;
            }
            case PersonMovementDirections::SOUTHEAST:
            {
                directionKey = PersonMovementDirections::NORTHWEST;
                break;
            }
            case PersonMovementDirections::SOUTH:
            {
                directionKey = PersonMovementDirections::NORTH;
                break;
            }
            case PersonMovementDirections::SOUTHWEST:
            {
                directionKey = PersonMovementDirections::NORTHEAST;
                break;
            }
            case PersonMovementDirections::WEST:
            {
                directionKey = PersonMovementDirections::EAST;
                break;
            }
            case PersonMovementDirections::NORTHWEST:
            {
                directionKey = PersonMovementDirections::SOUTHEAST;
                break;
            }
        }

        ev << "[Sensor Node #" << this->self << "::WsnLogic::getDestFromSenderNodeID] A directionKey é #" << directionKey << endl;
        return directionKey;
    }
    else
    {
        ev << "[Sensor Node #" << this->self << "::WsnLogic::getDestFromSenderNodeID] Não deveria acontecer." << endl;
        opp_error("Can't happen!");
    }
}
