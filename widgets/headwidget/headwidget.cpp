/*
 * Elapse Client
 * Copyright (C) 2015 Simon Knopp
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Additional permission under GNU GPL version 3 section 7:
 *
 * The licensors of this Program grant you additional permission to link
 * this Program with other modules by means of the provided plugin
 * interface and to convey the resulting work, regardless of the license
 * terms of the other modules.
 */

#include <math.h>
#include <QtWidgets>
#include <QtOpenGL>
#include "util/ratelimiter.h"
#include "headmesh.h"
#include "headwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

static const QColor headColour = QColor::fromCmykF(0.0, 0.1, 0.2, 0.3);
static const QColor bgndColour = QColor::fromRgb(30, 30, 60);

namespace elapse { namespace widgets {

class HeadWidgetPrivate
{
public:
    HeadMesh head;
    float xRot = 0;
    float yRot = 0;
    float zRot = 0;
    RateLimiter update {30};
};

/*!
 * Create a new HeadWidget as a child of the given \a parent.
 */
HeadWidget::HeadWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
    d_ptr(new HeadWidgetPrivate)
{
    connect(&d_ptr->update, SIGNAL(ready()), SLOT(updateGL()));
}

HeadWidget::~HeadWidget()
{
}

QSize HeadWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize HeadWidget::sizeHint() const
{
    return QSize(200, 250);
}

static float rad2deg(float radians)
{
    float degrees = radians * 180 / M_PI;

    while (degrees < 0)
        degrees += 360;
    while (degrees > 360)
        degrees -= 360;

    return degrees;
}

/*!
 * Set the rotation angle about the x axis in \a radians.
 */
void HeadWidget::setXRotation(float radians)
{
    Q_D(HeadWidget);
    float angle = rad2deg(radians);
    if (angle != d->xRot) {
        d->xRot = angle;
        d->update();
    }
}

/*!
 * Set the rotation angle about the y axis in \a radians.
 */
void HeadWidget::setYRotation(float radians)
{
    Q_D(HeadWidget);
    float angle = rad2deg(radians);
    if (angle != d->yRot) {
        d->yRot = angle;
        d->update();
    }
}

/*!
 * Set the rotation angle about the z axis in \a radians.
 */
void HeadWidget::setZRotation(float radians)
{
    Q_D(HeadWidget);
    float angle = rad2deg(radians);
    if (angle != d->zRot) {
        d->zRot = angle;
        d->update();
    }
}

/*!
 * Load the head mesh and initialise OpenGL.
 */
void HeadWidget::initializeGL()
{
    Q_D(HeadWidget);
    qglClearColor(bgndColour);
    d->head.setColor(headColour);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

/*!
 * Render the head mesh with the appropriate rotation.
 */
void HeadWidget::paintGL()
{
    Q_D(HeadWidget);
    // Centre of rotation
    float x0 = 0, y0 = -0.3, z0 = -0.2;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glTranslatef(x0, y0, z0);
    // Intrinsic rotation about (x0,y0,z0)
    // The rotation vectors are weird because the OpenGL axes are different to
    // the axes assigned to the (real) head in the Elapse framework.
    glRotatef(d->zRot, 0.0, 1.0, 0.0);
    glRotatef(d->yRot, 0.0, 0.0, 1.0);
    glRotatef(d->xRot,-1.0, 0.0, 0.0);
    glTranslatef(-x0, -y0, -z0);
    d->head.draw();
}

/*!
 * Handle widget resizing - center the head within an area of \a width by
 * \a height.
 */
void HeadWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

}} // namespace elapse::widgets
