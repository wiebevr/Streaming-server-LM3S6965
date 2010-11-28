#ifndef VIDEO_VIEWER_H
#define VIDEO_VIEWER_H

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QVector>
#include <QVector2D>
#include <QMatrix2x2>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

class VideoViewer
    : public QGLWidget
{
    Q_OBJECT

    public:
        VideoViewer(QWidget *parent = 0);
        ~VideoViewer();

    public slots:
        void updateTexture(IplImage *frame);
        void updateAspectRatio();

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);
    private:
        void makeShaders();
        void makeGeometry();


        QVector<QVector2D> _vertexArray;
        QGLShaderProgram *_shaderProgram;
        GLuint _textureId;
        QImage _image;
        QMatrix2x2 _matrix;
        IplImage *_frame;


};
#endif //VIDEO_VIEWER_H
