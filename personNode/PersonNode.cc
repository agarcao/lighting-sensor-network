/*
 * @author André Garção
 * @version 1.0
 */

#include "PersonNode.h"
#include "PersonsPhysicalProcess.h"

// The module class needs to be registered with OMNeT++
Define_Module(PersonNode);

void PersonNode::initialize()
{
    int field_x = this->par("field_x");
    int field_y = this->par("field_y");

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
    this->xCoor = rand() % field_x;
    this->yCoor = rand() % field_y;

    //this->xCoor = 15;
    //this->yCoor = 15;

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
        int direction = this->changeDirection();

        this->xCoor = (this->xCoor + (this->getDirectionX(direction) * this->moveVelocity)) % field_x;
        this->yCoor = (this->yCoor + (this->getDirectionY(direction) * this->moveVelocity)) % field_y;

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

int PersonNode::changeDirection()
{
    bool trueOrFalse = (rand() % 100) > (100 - (this->changeDirectionProbability * 100));
    int directions[4] = {PersonMovementDirections::NORTH, PersonMovementDirections::EAST, PersonMovementDirections::SOUTH, PersonMovementDirections::WEST};


    // Se for para mudar
    if (trueOrFalse)
    {
        int randomNumber = rand() % 4;
        int direction = directions[randomNumber];
        if (direction != this->movementDirection) {
            return direction;
        }
        else {
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
