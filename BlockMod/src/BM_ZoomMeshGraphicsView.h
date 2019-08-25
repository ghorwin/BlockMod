#ifndef BM_ZoomMeshGraphicsViewH
#define BM_ZoomMeshGraphicsViewH

#include <QGraphicsView>
#include <QVarLengthArray>
#include <QWidget>
#include <QColor>

namespace BLOCKMOD {

/*! Basic 2D graphics view that provides a mesh and
	a zoom functionality.
*/
class ZoomMeshGraphicsView : public QGraphicsView {
	Q_OBJECT
public:
	explicit ZoomMeshGraphicsView(QWidget *parent=0);

public slots:

	/*! Sets the color for the grid. */
	void setGridColor( QColor color );

	/*! Sets the grid visibilty state. */
	void setGridEnabled( bool );

	/*! Increases zoom level. */
	void zoomIn();

	/*! Decreases zoom level. */
	void zoomOut();

	/*! Sets new zoom level. */
	void setZoomLevel(int zoomLevel);

	/*! Returns current zoom level. */
	int zoomLevel() const { return m_zoomLevel; }

	/*! Reset the zoom to 0. */
	void resetZoom();

	/*! Sets a new grid step/grid spacing (this is the major grid).
		A minor grid is shown with 10 % smaller grid spacing.
		\param gridStep Grid spacing in [m], must be > 0.

		\note Grid lines will not be printed when gridStep*resolution*scene scale is < 2 Pixel.
	*/
	void setGridStep(double gridStep);

	/*! Sets resolution in [pix/m].
		Resolution should be set directly after construction.
		\param res The new resolution in [pix/m].
	*/
	void setResolution(double res);

protected:
	/*! set the zoom center. */
	void mouseMoveEvent(QMouseEvent *event);

	/*! Enables the zoom. */
	void wheelEvent(QWheelEvent *i_event);

	/*! Draws the mesh. */
	void paintEvent(QPaintEvent *i_event);

	/*! This event is called from this class and can be used in derived classes to
		react on changes to the zoom factor. */
	virtual void changeResolutionEvent() {}

	/*! Defines resolution to be used with graphics scene in [pix/m]. */
	double	m_resolution;

	/*! The grid distance in [m]. */
	double	m_gridStep;

	/*! Defined as true if grid should be painted. */
	bool	m_gridEnabled;

	/*! An integer defining the level of zoom (- zoom in, + zoom out). */
	int		m_zoomLevel;

private:

	/*! The current mouse point. */
	QPointF							m_pos;

	/*! The major grid lines. */
	QVarLengthArray<QLineF, 100>	m_majorGrid;

	/*! The minor grid lines. */
	QVarLengthArray<QLineF, 100>	m_minorGrid;

	/*! The grid color (major grid). */
	QColor							m_gridColor;

	/*! Cached reference point used during last grid update. */
	QPointF							m_pLast;
	/*! Cached grid spacing used during last grid update. */
	double							m_gridSpacingPixLast;
	/*! Cached window size used during last grid update. */
	QSize							m_windowSizeLast;

};

} // namespace BLOCKMOD


#endif // BM_ZoomMeshGraphicsViewH
