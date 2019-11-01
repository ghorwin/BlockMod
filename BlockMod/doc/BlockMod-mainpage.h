/*!

\mainpage Overview of the Block Modeling Toolkit

\tableofcontents

\section intro_sec What is the BlockMod library?

BlockMod is a library for developing block modeling software. Block modeling is
a way of describing a component simulation model that is broken down into connected
blocks. Connections are causal, from output to input.

The library handles the underlying data structure to describe the connections
and provides code for graphical visualization and customization of such
block models.

\subsection key_features Key Features

The library contains a data model for storing all geometrical information needed to plot
and organize a block schematic.

The visualization of the data model is done using QGraphicsView functionality. The library contains
implementations of different QGraphicsItem classes for the various entities.
The created blocks, sockets and connectors can be rendered on screen, and also exported to different
graphics formats and printed. The library contains graphics item classes for each entity in the data model.

\subsection goals Goals, Vision and Design Requirements

Block-based networks are needed for quite a few applications. When developing such an application, where users are allowed to create their own networks, you may need the following functionality:

- drawing of a network of blocks and connections between blocks
- zooming/panning in the view
- showing sockets on the blocks (optionally with their labels)

User interaction:

- selecting/deselecting blocks and connections
- connect blocks by dragging lines from socket to socket
- moving blocks around, hereby keeping the connections and adjusting their geometry automatically

Advanced features:

- defining block geometry and their socket types and positions; when modifying existing blocks keep connections previously made (except when a socket is removed)

\subsection API_concept Overview of the library functionality

- access/retrieve data structure (the network)
- create/manipulate data structure via API (adding/removing blocks and connectors)
- serialize/deserialize data structure (read/write to xml stream and/or file)

Customizability:

- override default appearance/drawing of blocks, sockets and connectors
- create blocks with additional side effects (e.g. mouse hovering shows auxilliary attributes)

Auxilliary features:

- export view of network as vector graphics image/print to printer/PDF
- adjust style/view options

\section download Download, building, examples

1. Clone the repository https://github.com/ghorwin/BlockMod
2. enter the build/cmake directory
3. run `build.sh` or `build_x64.bat` (the scripts set environmental variables for Qt and Visual Studio 2015 on Windows, adjust those as needed before running the scripts)

\subsection Examples

* `SerializationTest` - reads/writes a network from file (in XML format); creates a new network on first run
* `ShowNetworkTest` - Creates a network and displays it in the graphics view
* `BlockModDemo` - A demonstration of all the capabilities of the BlockMod library; block/connector moving, creating/removing connections, adding/removing blocks, ...

\section userInteraction Usage/User Interaction

The graphics scene and graphics view provided by the library supports the following user interactions:

- Scroll wheel - zoom in/zoom out
- single left click - select block/connector
- CTRL + left click - select multiple blocks (but only one connector can be selected at a time!)
- click and hold left mouse button and drag item - move blocks and connector segments

When in connection mode (started with API call, see below):

- hovering over outlet socket with highlight the socket
- click and hold left mouse button on outlet socket will start a new connection
- drag to free inlet socket (highlights when hovering over) and release mouse button to create connection
- right-click to leave connection mode



\section API Programming Interface (API)


\subsection quickStart Quick Start

Compile the library (and set include/linker paths accordingly) or copy the BlockMod library source files into your project.

Simple application to show a network.
\code
// import the SceneManager and ZoomingMeshGraphicsView from BlockMod library
#include <BM_ZoomMeshGraphicsView.h>
#include <BM_SceneManager.h>
#include <BM_Globals.h> // for GridSpacing

// ...

// create network and read from file
BLOCKMOD::Network network;
network.readXML("demo.bm");

// create scene
BLOCKMOD::SceneManager * networkManager = new BLOCKMOD::SceneManager;
networkManager->setNetwork(network); // network is now known and managed by scene manager

// create view and adjust background grid
BLOCKMOD::ZoomMeshGraphicsView * w = new BLOCKMOD::ZoomMeshGraphicsView;
w->setResolution(1);
w->setGridStep(10*BLOCKMOD::Globals::GridSpacing); // align to grid

// set scene and show view
w->setScene(networkManager);
w->show();
\endcode

\subsection networkManipulation Manipulating the network

\subsubsection addingBlocks Adding/removing blocks

\code
const double GS = BLOCKMOD::Globals::GridSpacing;

BLOCKMOD::Block b;
b.m_name = "New Block"; // give it a name
b.m_size = QSizeF(5*GS, 10*GS); // size - aligned to grid
b.m_pos = QPointF(20*GS, 5*GS); // position - aligned to grid

// add an inlet socket to the left of the block
BLOCKMOD::Socket s;
s.m_name = "Inlet 1";
s.m_inlet = true;
s.m_pos = QPointF(0, 2*GS);
s.m_orientation = Qt::Horizontal;
b.m_sockets.append(s);

// add an outlet socket to the right
BLOCKMOD::Socket s2("Outlet 2", QPointF(b.m_size.width(), 2*GS), Qt::Horizontal, false);
b.m_sockets.append(s2);

// finally add the block to the managed network
networkManager->addBlock(b);
\endcode

Removing a block can be done via ```SceneManager::removeBlock(...)```.

\subsubsection retrievingNetwork Retrieving the network and setting a new network

\code
// get the network
BLOCKMOD::Network n = networkManager->network();

// set the network
networkManager->setNetwork(n);
\endcode

That's also, how you implement undo/redo functionality. Just take the network after every operation
and store it in an undo action. During an undo, simply swap out the cached and new network.

\subsubsection addingConnectors Adding/Removing connectors

To add a new connector, create a new connector object ```BLOCKMOD::Connector``` and set name,
source and target socket identifiers. Flat socket identifiers have the format: `<block name>.<socket name>`. Hence, block names **must not have a dot character in the name**. Block names and socket names must match existing blocks/sockets in the network, before you call ```SceneManager::addConnector()```.

Use ```SceneManager::removeConnector(...)``` to remove an existing connector.

\subsubsection enterConnectionMode Starting connection mode

In _connection mode_ the scene shows a cross mouse cursor and allows new connections to be made between outlet and inlet sockets.

Call ```SceneManager::enableConnectionMode()``` to put the scene into connection mode.


\section implementationGuide Implementation

Data structure `Network` contains `Block` definitions. Each block may have several sockets, either input or output sockets. Sockets may be connected via `Connectors`, which also belong to the network.
Blocks, their sockets and connector can have custom properties, managed by property lists.
The data structure can be serialized in a json/xml representation.

\subsection data_model_functionality Data Model Functionality

The data model stores all information that is needed to reproduce the block diagram, and
further some meta data for the display (zoom, position, grid spacing, etc.). The data
model can be serialized into an XML data structure. The data model stores by default only
data needed for reconstructing the geometry and the view properties.

The key entities are:

- BLOCKMOD::Block : The major building block of the block diagram
- BLOCKMOD::Socket : Either input or output socket, belongs to an individual block
- BLOCKMOD::Connector : A polygon from output to input socket
- BLOCKMOD::Network : The compound class that holds all data belonging to a network.

That means, the entities BLOCKMOD::Block, BLOCKMOD::Socket and BLOCKMOD::Connector hold all data relevant for
drawing the respective graphical representation.



\subsection visualization Visualization

The network is visualized in a QGraphicsView. The SceneManager generates/maintains the scene corresponding to a network. The scene manager provides default functions for generating graphics items for blocks, their sockets and the connectors. These generation functions can be overloaded to generate custom graphics items (with different visual appearance).

Default blocks can be simple rectangles, or svg/png icons. Default sockets are triangles or semi-circles, depending on inlet/outlet types.

The visualization can be switched between _connector-view_ and _connection graph_. The latter will not show sockets, but merely arrows between components with arrow heads indicating data flow direction.

\subsubsection ui User Interaction

The framework can handle mouse-move events for blocks. This will automatically move connected connectors as well. Also, the different parts of a connector can be moved as well.

\subsubsection dataSync Data synchronization

When graphics items are moved, the underlying data structure is updated directly (graphics items know their data storage counter parts via pointer links). For example, when moving a block, the connected connector items are modified (and the associated connector data structure as well).

When network is changed in the data structure, e.g. by adding/removing a block, the scene manager synchronizes to the new data structure, hereby only modifying those properties, that have actually changed.


## Implementation

Data members/objects refer to each other via pointer links. Care has to be taken, when modifying vectors/lists so that the pointer links are updated accordingly.

Note: when any of the vectors of the network's data structures change, the memory location of these blocks/connectors may become invalid. Graphics items still pointing to that memory may cause access violations. Thus, any change of network's data structures must yield an update of the respective Graphics items.

Hence, the scene manager also handles any changes to the underlying data structures and provides functions for adding/removing blocks, connectors, sockets etc.


\subsection routing_support Connector Routing

A logic is implemented that allows meaningful routing of connector polygons. A rule set is defined that modifies the
polygon when end points/mid points are moved or new points are added.


\subsection extending Extending the library

The core functionality of the library is limited to drawing SVG and bitmap images. Also, the appearance of connectors
and sockets is fixed.

Creating themes:
- for each block type an image/SVG must be specified (size?)
- an image is needed for inlet/outlet sockets, in disconnected and connected state, respectively
- style information about connector drawing (width, color, pen)


Users may want to extend the library because:
- unforeseen appearance wishes (shadows, outlines, script-based display)
-

*/


/*! \brief The namespace BLOCKMOD contains all functions/classes/data types of the BLOCKMOD library.
*/
namespace BLOCKMOD {
}

