#include "video_viewer.h"

#include <QString>
#include <QGLShader>
#include <QImage>
#include <QVector2D>
#include <QMatrix2x2>

VideoViewer::VideoViewer(QWidget *parent)
    : QGLWidget(parent)
{
    _shaderProgram = NULL;
    _frame = cvLoadImage("Tux.png");
}

VideoViewer::~VideoViewer()
{
}

void VideoViewer::initializeGL()
{

    glEnable(GL_VERTEX_ARRAY);
    glEnable(GL_TEXTURE_2D);

    makeShaders();
    makeGeometry();
    //updateTexture();

    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    qDebug() << _textureId;

}

void VideoViewer::paintGL()
{
    qglClearColor(Qt::red);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, 
            _frame->nChannels == 1 ? GL_LUMINANCE : GL_RGB,
            _frame->width, _frame->height, 0, 
            _frame->nChannels == 1 ? GL_LUMINANCE : GL_BGR,
            GL_UNSIGNED_BYTE, _frame->imageData);

    _shaderProgram->setUniformValue("texture", 0);
    
    _shaderProgram->setUniformValue("matrix", 
            _matrix);

    _shaderProgram->setAttributeArray("position", _vertexArray.constData());
    _shaderProgram->enableAttributeArray("position");

    glBindTexture(GL_TEXTURE_2D, _textureId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    _shaderProgram->disableAttributeArray("position");
}

void VideoViewer::updateAspectRatio()
{
    resizeGL(width(), height());
    qDebug() << "update AA";
}

void VideoViewer::resizeGL(int width, int height)
{
    double projectionMatrixValues[4];
    double scaleMatrixValues[4];

    if(height * _frame->width < width * _frame->height)
    {
        projectionMatrixValues[0] = (double) height / (double)width;
        projectionMatrixValues[1] = 0.0;
        projectionMatrixValues[2] = 0.0;
        projectionMatrixValues[3] = 1.0;

        scaleMatrixValues[0] = (double) _frame->width / (double)_frame->height;
        scaleMatrixValues[1] = 0.0;
        scaleMatrixValues[2] = 0.0;
        scaleMatrixValues[3] = 1.0;
    }
    else 
    {
        projectionMatrixValues[0] = 1.0;
        projectionMatrixValues[1] = 0.0;
        projectionMatrixValues[2] = 0.0;
        projectionMatrixValues[3] = (double) width / (double)height;

        scaleMatrixValues[0] = 1.0;
        scaleMatrixValues[1] = 0.0;
        scaleMatrixValues[2] = 0.0;
        scaleMatrixValues[3] = (double)_frame->height / (double)_frame->width;
    }

    _matrix = QMatrix2x2(projectionMatrixValues) *
              QMatrix2x2(scaleMatrixValues);

    glViewport(0, 0, width, height);

}

void VideoViewer::updateTexture(IplImage *frame)
{
    _frame = frame;
    updateGL();
}

void VideoViewer::makeGeometry()
{
    _vertexArray.append(QVector2D(-1.0f, -1.0f));
    _vertexArray.append(QVector2D(1.0f, -1.0f));
    _vertexArray.append(QVector2D(-1.0f, 1.0f));
    _vertexArray.append(QVector2D(1.0f, 1.0f));
}

void VideoViewer::makeShaders()
{
    _shaderProgram = new QGLShaderProgram(this);
    QGLShader *vertexShader = new QGLShader(QGLShader::Vertex, this);
    QGLShader *fragmentShader = new QGLShader(QGLShader::Fragment, this);
    
    QString vertexShaderSource;
    QString fragmentShaderSource;

    vertexShaderSource.append("#version 110\n");
    vertexShaderSource.append("attribute vec2 position;\n");
    vertexShaderSource.append("uniform mat2 matrix;\n");
    vertexShaderSource.append("varying vec2 texcoord;\n");
    vertexShaderSource.append("void main()\n");
    vertexShaderSource.append("{\n");
    vertexShaderSource.append(
            "gl_Position = vec4(matrix * position, 0.0, 1.0);\n");
    vertexShaderSource.append("texcoord = vec2(position.x, -position.y) "
            "* vec2(0.5) + vec2(0.5);\n");
    vertexShaderSource.append("}\n");
    
    fragmentShaderSource.append("#version 110\n");
    fragmentShaderSource.append("uniform sampler2D texture;\n");
    fragmentShaderSource.append("varying vec2 texcoord;\n");
    fragmentShaderSource.append("void main()\n");
    fragmentShaderSource.append("{\n");
    fragmentShaderSource.append(
            "gl_FragColor = texture2D(texture, texcoord);\n");
    fragmentShaderSource.append("}\n");

    vertexShader->compileSourceCode(vertexShaderSource);
    fragmentShader->compileSourceCode(fragmentShaderSource);

    if (!vertexShader->isCompiled())
    {
        qDebug()
            << "Could not compile vertex shader:\n"
            << vertexShader->log();
    }

    if (!fragmentShader->isCompiled())
    {
        qDebug()
            << "Could not compile fragment shader:\n"
            << fragmentShader->log();
    }

    _shaderProgram->addShader(fragmentShader);
    _shaderProgram->addShader(vertexShader);

    _shaderProgram->link();
    _shaderProgram->bind();

}
