/*
 * @author André Garção
 * @version 1.0
 */

#include "Wall.h"

// The module class needs to be registered with OMNeT++
Define_Module(Wall);

void Wall::initialize()
{
    // TODO: Temos que posicionar a Wall
    // 1. Precisamos do nuero de nó (que por sua vez tbm nos dizem qtas celulas existem) para ver em qual delas fazemos o display do obstaculo
    this->numNodes = this->getAncestorPar("numNodes");

    int cellToPutWall = random() % this->numNodes;

    ev << "[Obstaculo #" << this->getIndex() << "::Wall::initialize] Esta é para a cell #" << cellToPutWall  << endl;

    // 2. Precisamos agora de saber qual é a direção do obstaculo
    int wallPosition = random() % 4;

    ev << "[Obstaculo #" << this->getIndex() << "::Wall::initialize] Calhou a posição #" << wallPosition  << endl;

    // 3. Vamos agora calcular onde por o obstaculo no GUI
    // 3.1. Posição inicial
    int horizontalCells = this->getAncestorPar("horizontal_cells");
    int verticalCells = this->getAncestorPar("vertical_cells");
    int cellSize = this->getAncestorPar("cell_size");

    int x = (cellToPutWall % horizontalCells) * cellSize;
    int y = (cellToPutWall / verticalCells) * cellSize;

    ev << "[Obstaculo #" << this->getIndex() << "::Wall::initialize] Depois de calculado x = " << x << "e y = " << y << ". Mas ainda falta. "<< endl;

    // 3.2. Dependendo agora da posição temos que fazer modificações
    int heigh = 1;
    int witdh = 1;

    switch (wallPosition)
    {
        case 0:
        {
            witdh = cellSize;
            x = x + (cellSize / 2);
            break;
        }
        case 1:
        {
            heigh = cellSize;
            x = x + cellSize;
            y = y + (cellSize / 2);
            break;
        }
        case 2:
        {
            witdh = cellSize;
            y = y + cellSize;
            x = x + (cellSize / 2);
            break;
        }
        case 3:
        {
            heigh = cellSize;
            y = y + (cellSize / 2);
            break;
        }
    }

    ev << "[Obstaculo #" << this->getIndex() << "::Wall::initialize] Depois dos calculos finais ficou em (" << x << ", " << y << ") com width = " << witdh
            << " e height = " << heigh << endl;

    // 4. Vamos agora meter o display
    char s[10];
    cDisplayString &nodeDS = this->getDisplayString();

    // Set x
    sprintf(s,"%d", x);
    nodeDS.setTagArg("p",0, s);

    // Set y
    sprintf(s,"%d", y);
    nodeDS.setTagArg("p",1, s);

    // Set y
    sprintf(s,"%d", witdh);
    nodeDS.setTagArg("b",0, s);

    // Set y
    sprintf(s,"%d", heigh);
    nodeDS.setTagArg("b",1, s);
}

void Wall::handleMessage(cMessage *msg)
{
}

void Wall::finish()
{

}
