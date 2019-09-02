/*	BSD 3-Clause License

	This file is part of the BlockMod Library.

	Copyright (c) 2019, Andreas Nicolai
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	   contributors may be used to endorse or promote products derived from
	   this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
	/*! Overloaded to set cross/arrow cursor, depending on connection-mode state of scene. */
	void enterEvent(QEvent *event) override;

	/*! Overloaded to set arrow cursor. */
	void leaveEvent(QEvent *event) override;

	/*! set the zoom center. */
	void mouseMoveEvent(QMouseEvent *event) override;

	/*! Enables the zoom. */
	void wheelEvent(QWheelEvent *i_event) override;

	/*! Draws the mesh. */
	void paintEvent(QPaintEvent *i_event) override;

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
