#include "BM_ZoomMeshGraphicsView.h"

#include <QWheelEvent>
#include <QList>
#include <QGraphicsItem>
#include <QDebug>
#include <cmath>

namespace BLOCKMOD {

ZoomMeshGraphicsView::ZoomMeshGraphicsView(QWidget *parent) :
	QGraphicsView(parent),
	m_resolution(1000), // 1000 px/m
	m_gridStep(0.1),
	m_gridEnabled( true ),
	m_zoomLevel(0),
	m_gridColor( 175, 175, 255 ),
	m_gridSpacingPixLast(0)
{
	setTransformationAnchor(AnchorUnderMouse);

	// this saves our overlays while drawing
	setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
}



void ZoomMeshGraphicsView::wheelEvent(QWheelEvent *i_event){

	if (i_event->delta() < 0) {
		zoomOut();
	}
	else {
		zoomIn();
	}
	i_event->accept();
}



void ZoomMeshGraphicsView::paintEvent(QPaintEvent *i_event){

	if (m_gridEnabled) {

		// get rect
		QPainter p(viewport());
		QPointF p1 = mapFromScene( QPointF(0,0) );

		// compute nominal pixels using resolution
		double gridSpacingPix = m_resolution*m_gridStep;

		// apply scaling
		double scaleFactor = matrix().m11();
		gridSpacingPix *= scaleFactor;

		// get dimensions
		int h = height();
		int w = width();
		QSize windowSize(w,h);

		// if new reference point, grid scale factor and window size still match cached values, do
		// not recompute grid lines
		if (gridSpacingPix != m_gridSpacingPixLast ||
			p1 != m_pLast ||
			windowSize != m_windowSizeLast)
		{

			// cache values used for calculation
			m_pLast = p1;
			m_gridSpacingPixLast = gridSpacingPix;
			m_windowSizeLast = windowSize;

			// *** major grid ***

			// clear major grid lines
			m_majorGrid.clear();

			// only regenerate mesh and draw grid if spacing is big enough
			if (gridSpacingPix >= 5) {

				// calculate offset - we start at 0 pix in scaled scene coordinates
				int linesXTillView = std::floor(-p1.x()/gridSpacingPix + 1);
				double offsetX = linesXTillView*gridSpacingPix + p1.x();

				int linesYTillView = std::floor(-p1.y()/gridSpacingPix + 1);
				double offsetY = linesYTillView*gridSpacingPix + p1.y();

				for (double x = offsetX; x < w; x += gridSpacingPix) {
					m_majorGrid.append(QLineF(x, 0, x, h));
				}
				for (double y = offsetY; y < h; y += gridSpacingPix) {
					m_majorGrid.append(QLineF(0, y, w, y));
				}
			}


			// *** minor grid ***

			// compute nominal pixels using resolution, this time for minor grid
			gridSpacingPix = m_resolution*m_gridStep*0.1;

			// apply scaling
			scaleFactor = matrix().m11();
			gridSpacingPix *= scaleFactor;

			// clear minor grid lines
			m_minorGrid.clear();

			// only draw grid if spacing is big enough
			if (gridSpacingPix >= 5) {

				// calculate offset - we start at 0 pix in scaled scene coordinates
				int linesXTillView = std::floor(-p1.x()/gridSpacingPix + 1);
				double offsetX = linesXTillView*gridSpacingPix + p1.x();

				int linesYTillView = std::floor(-p1.y()/gridSpacingPix + 1);
				double offsetY = linesYTillView*gridSpacingPix + p1.y();

				// create a vector of lines to draw
				m_minorGrid.clear();

				for (double x = offsetX; x < w; x += gridSpacingPix) {
					m_minorGrid.append(QLineF(x, 0, x, h));
				}
				for (double y = offsetY; y < h; y += gridSpacingPix) {
					m_minorGrid.append(QLineF(0, y, w, y));
				}

			}
		}
		// we paint in view coordinates
		p.setPen( QColor(220,220,255) );
		p.drawLines(m_minorGrid.data(), m_minorGrid.size());

		// we paint in view coordinates
		p.setPen( m_gridColor );
		p.drawLines(m_majorGrid.data(), m_majorGrid.size());

	}

	QGraphicsView::paintEvent(i_event);
}


void ZoomMeshGraphicsView::mouseMoveEvent(QMouseEvent *i_event) {
	QGraphicsView::mouseMoveEvent(i_event);
	m_pos = mapToScene(i_event->pos());
	viewport()->update();
}


void ZoomMeshGraphicsView::setGridColor( QColor color ) {
	m_gridColor = color;
	viewport()->update();
}


void ZoomMeshGraphicsView::setGridEnabled( bool enabled ){
	m_gridEnabled = enabled;
	viewport()->update();
}


void ZoomMeshGraphicsView::zoomIn() {

	m_zoomLevel = std::min(m_zoomLevel+1, 3000);
	double factor = std::pow(10,m_zoomLevel/20.0);
	QTransform m(factor, 0, 0, factor, 0, 0);
	setTransform(m);
	changeResolutionEvent();

}


void ZoomMeshGraphicsView::zoomOut() {

	m_zoomLevel = std::max(m_zoomLevel-1, -3000);
	double factor = std::pow(10,m_zoomLevel/20.0);
	QTransform m(factor, 0, 0, factor, 0, 0);
	setTransform(m);
	changeResolutionEvent();

}


void ZoomMeshGraphicsView::setZoomLevel(int zoomLevel) {

	m_zoomLevel = std::max(zoomLevel, -3000);
	m_zoomLevel = std::min(m_zoomLevel, 3000);

	double factor = std::pow(10,m_zoomLevel/20.0);
	QTransform m(factor, 0, 0, factor, 0, 0);
	setTransform(m);
	changeResolutionEvent();

}


void ZoomMeshGraphicsView::resetZoom() {

	m_zoomLevel = 0;
	resetTransform();

}


void ZoomMeshGraphicsView::setGridStep(double gridStep) {
	if (gridStep <=0) return;
	m_gridStep = gridStep;
	viewport()->update();
}


void ZoomMeshGraphicsView::setResolution(double res) {
	if (res <=0) return;
	m_resolution = res;
	viewport()->update();
}

} // namespace BLOCKMOD
