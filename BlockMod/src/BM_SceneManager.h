#ifndef BM_SceneManagerH
#define BM_SceneManagerH

#include <QGraphicsScene>
class QGraphicsItem;

namespace BLOCKMOD {

class Network;

class SceneManager : public QGraphicsScene {
	Q_OBJECT
public:
	explicit SceneManager(QObject *parent = 0);

	void setNetwork(Network & network);


private:
	/*! The network that we manage. */
	Network			*m_network;

	/*! The graphics items that we show on the scene. */
	QList<QGraphicsItem*>	m_items;
};

} // namespace BLOCKMOD

#endif // BM_SceneManagerH
