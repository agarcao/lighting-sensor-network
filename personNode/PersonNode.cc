/*
 * @author André Garção
 * @version 1.0
 */

#include "PersonNode.h"
#include "Wall.h"
#include "PersonsPhysicalProcess.h"

// The module class needs to be registered with OMNeT++
Define_Module(PersonNode);

void PersonNode::initialize()
{
    // Parametros que vêm do modulo
    this->timeBetweenMoves = par("timeBetweenMoves");
    this->moveVelocity = par("moveVelocity");
    this->changeDirectionProbability = par("changeDirectionProbability");

    int directionInt = this->checkDirection(par("startDirection"));
    if (directionInt) {
        this->movementDirection = directionInt;
    }
    else {
        opp_error("Movement direction of the node is not valid");
    }

    // Random Values for initial Person position on the map
    //this->xCoor = rand() % field_x;
    //this->yCoor = rand() % field_y;

    this->xCoor = 15;
    this->yCoor = 225;

    ev << "Eu (Person Node #" << this->getIndex() << " ) inicializei-me na posição ("
            << this->xCoor << ", " << this->yCoor << ")" << endl;

    char s[10];
    cDisplayString &nodeDS = getDisplayString();

    // Set x
    sprintf(s,"%d",this->xCoor);
    nodeDS.setTagArg("p",0,s);

    // Set y
    sprintf(s,"%d",this->yCoor);
    nodeDS.setTagArg("p",1,s);

    // Finaly send message to itself to move
    this->event = new cMessage("move");
    scheduleAt(simTime() + this->par("timeBetweenMoves"), this->event);

    ev << "Send move message" << endl;
}

void PersonNode::handleMessage(cMessage *msg)
{
    // Se é uma mensagem do tipo 'move' o person node tem que se mover
    if (strcmp("move", msg->getName())== 0)
    {
        ev << "Eu (Person Node #" << this->getIndex() << " ) recebi msg para mover" << endl;

        int field_x = this->par("field_x");
        int field_y = this->par("field_y");

        // Mudamos a posição da pessoa (person node)
        this->movementDirection = this->changeDirection();
        ev << "[Person Node #" << this->getIndex() << "::handleMessage] Direcao #" << this->movementDirection << endl;

        // Tem que se verificar se com o movimento a pessoa (personNode) não ultrapassa os limites do field.
        // Além disso também precisamos de ver se não bate num obstaculo. Se ultrapassar ou bater tem que mudar a direção do movimento
        while (this->xCoor + (this->getDirectionX(this->movementDirection) * this->moveVelocity) > field_x
                || this->xCoor + (this->getDirectionX(this->movementDirection) * this->moveVelocity) < 0
                || this->yCoor + (this->getDirectionY(this->movementDirection) * this->moveVelocity) > field_y
                || this->yCoor + (this->getDirectionY(this->movementDirection) * this->moveVelocity) < 0
                || this->bumpObstacle())
        {
            ev << "[Person Node #" << this->getIndex() << "::handleMessage] xCoord: " << this->xCoor + (this->getDirectionX(this->movementDirection) * this->moveVelocity) << endl;
            ev << "[Person Node #" << this->getIndex() << "::handleMessage] yCoor: " << this->yCoor + (this->getDirectionY(this->movementDirection) * this->moveVelocity) << endl;
            ev << "[Person Node #" << this->getIndex() << "::handleMessage] Com esta direção saimos do field ou batemos num obstaculo" << endl;
            this->movementDirection = this->changeDirection(true);
            ev << "[Person Node #" << this->getIndex() << "::handleMessage] Movement direction é agr #" << this->movementDirection << endl;
        }

        this->xCoor = (this->xCoor + (this->getDirectionX(this->movementDirection) * this->moveVelocity)) % field_x;
        this->yCoor = (this->yCoor + (this->getDirectionY(this->movementDirection) * this->moveVelocity)) % field_y;

        ev << "Eu (Person Node #" << this->getIndex() << " ) Move to (" << this->xCoor << ", " <<  this->yCoor << ")" << endl;

        // Verificar se o nó com a nova posição está dentro do raio de deteção de algum sensor node
        cModule *personsPhysicalProcess = this->getParentModule()->getSubmodule("physicalProcess", 0);
        PersonsPhysicalProcess *ppp = check_and_cast<PersonsPhysicalProcess *>(personsPhysicalProcess);

        ppp->checkPersonNodeDetection(this);

        char s[10];
        cDisplayString &nodeDS = getDisplayString();

        // Set x
        sprintf(s,"%d",this->xCoor);
        nodeDS.setTagArg("p",0,s);

        sprintf(s,"%d",this->yCoor);
        nodeDS.setTagArg("p",1,s);

        ev << "Eu (Person Node #" << this->getIndex() << " ) Send NEW move message" << endl;
        scheduleAt(simTime() + this->par("timeBetweenMoves"), this->event);
    }
}

void PersonNode::finish()
{
    cancelAndDelete(this->event);
}

// Helpers Functions
int PersonNode::checkDirection(string movementDirection)
{
    if(movementDirection == "North"){
        return PersonMovementDirections::NORTH;
    }
    else if(movementDirection == "East"){
        return PersonMovementDirections::EAST;
    }
    else if(movementDirection == "South"){
        return PersonMovementDirections::SOUTH;
    }
    else if(movementDirection == "West"){
        return PersonMovementDirections::WEST;
    }
    return 0;
}

int PersonNode::changeDirection(bool force)
{
    bool trueOrFalse = (rand() % 100) > (100 - (this->changeDirectionProbability * 100)) || force;
    int directions[4] = {PersonMovementDirections::NORTH, PersonMovementDirections::EAST, PersonMovementDirections::SOUTH, PersonMovementDirections::WEST};


    // Se for para mudar
    if (trueOrFalse)
    {
        ev << "[Person Node #" << this->getIndex() << "::changeDirection] Vamos mudar de direcção" << endl;
        int randomNumber = rand() % 4;
        int direction = directions[randomNumber];
        if (direction != this->movementDirection) {
            ev << "[Person Node #" << this->getIndex() << "::changeDirection] Mudamos para #" << direction << endl;
            return direction;
        }
        else {
            ev << "[Person Node #" << this->getIndex() << "::changeDirection] Direction = movement Direction" << direction << endl;
            return directions[(randomNumber + 1) % 4];
        }
    }
    else {
        return this->movementDirection;
    }
}

int PersonNode::getDirectionX(int direction)
{
    switch (direction)
    {
        case PersonMovementDirections::EAST:
        {
            return 1;
        }
        case PersonMovementDirections::WEST:
        {
            return -1;
        }
        case PersonMovementDirections::NORTH:
        case PersonMovementDirections::SOUTH:
        {
            return 0;
        }
    }
}

int PersonNode::getDirectionY(int direction)
{
    switch (direction)
    {
        case PersonMovementDirections::NORTH:
        {
            return 1;
        }
        case PersonMovementDirections::SOUTH:
        {
            return -1;
        }
        case PersonMovementDirections::EAST:
        case PersonMovementDirections::WEST:
        {
            return 0;
        }
    }
}



// Tell us if with the movement we bump an obstacle or not
bool PersonNode::bumpObstacle(void)
{
    // 1. Tenho que descobrir em que celula é que estou inicialmente e para onde vou
    // 1.1. Temos que ir buscar os parametros necessários a SensorNetwork
    int cellSize = this->getAncestorPar("cell_size");
    int horizontalCells = this->getAncestorPar("horizontal_cells");

    int cellInit = (this->yCoor / cellSize) * horizontalCells + (this->xCoor / cellSize);
    int cellEnd = ((this->yCoor + (this->getDirectionY(this->movementDirection) * this->moveVelocity)) / cellSize) * horizontalCells +
            ((this->xCoor + (this->getDirectionX(this->movementDirection) * this->moveVelocity)) / cellSize);

    ev << "[Person Node #" << this->getIndex() << "::bumpObstacle] Estou na cell #" << cellInit << " e vou para a cell #" << cellEnd << endl;

    // 2. Agora que já temos as cell de onde o nó vem e para onde o nó vai vamos ver se bate em algum obstaculo
    // 2.1. Temos que ter o numero de obstaculos que existem na simulação
    int numObstacles = this->getAncestorPar("numObstacles");

    int i;
    cModule *parent;
    Wall *wall;

    for (i = 0; i < numObstacles; i++)
    {
        // 2.2. Vamos buscar o modulo
        parent = this->getParentModule();
        wall = check_and_cast <Wall*>(parent->getSubmodule("obstacle", i));

        // 2.3. Vamos checkar se existe o bump
        if ((wall->betweenCells[0] == cellInit && wall->betweenCells[1] == cellEnd) ||
                (wall->betweenCells[1] == cellInit && wall->betweenCells[0] == cellEnd))
        {
            ev << "[Person Node #" << this->getIndex() << "::bumpObstacle] O obstaculo #" << i << " está no meu caminho" << endl;
            return true;
        }
    }

    ev << "[Person Node #" << this->getIndex() << "::bumpObstacle] Não existem obstaculos no meu caminho" << endl;
    return false;
}



