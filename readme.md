# BlockMod - a C++/Qt library for modelling of block-based networks and connector routing

## Basic Concepts

### Data structure
Data structure `Network` contains `Block` definitions. Each block may have several sockets, either input or output sockets. Sockets may be connected via `Connectors`, which also belong to the network.

Blocks, their sockets and connector can have custom properties, managed by property lists.

The data structure can be serialized in a json/xml representation.

### Visualization

The network is visualized in a QGraphicsView. The SceneManager generates/maintains the scene corresponding to a network. The scene manager provides default functions for generating graphics items for blocks, their sockets and the connectors. These generation functions can be overloaded to generate custom graphics items (with different visual appearance).

Default blocks can be simple rectangles, or svg/png icons. Default sockets are triangles or semi-circles, depending on inlet/outlet types.

The visualization can be switched between _connector-view_ and _connection graph_. The latter will not show sockets, but merely arrows between components with arrow heads indicating data flow direction.

### User Interaction

The framework can handle mouse-move events for blocks. This will automatically move connected connectors as well. Also, the different parts of a connector can be moved as well.

### Data synchronization

When graphics items are moved, the underlying data structure is updated directly (graphics items know their data storage counter parts via pointer links). For example, when moving a block, the connected connector items are modified (and the associated connector data structure as well).

When network is changed in the data structure, e.g. by adding/removing a block, the scene manager synchronizes to the new data structure, hereby only modifying those properties, that have actually changed.


## Implementation

Note: when any of the vectors of the network's data structures change, the memory location of these blocks/connectors may become invalid. Graphics items still pointing to that memory may cause access violations. Thus, any change of network's data structures must yield an update of the respective Graphics items.

Hence, the scene manager also handles any changes to the underlying data structures and provides functions for adding/removing blocks, connectors, sockets etc.
