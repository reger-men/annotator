#include "kalmanfilter.h"
#include "widget.h"

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Frame.h>
#include <QtGui/QPainter>
#include <QDebug>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <ctype.h>

using namespace Annotator::Plugins;

Annotator::Plugins::KalmanFilter::KalmanFilter()
{

}

KalmanFilter::~KalmanFilter()
{

}

QString KalmanFilter::getName()
{
    return "KalmanFilter";
}

QWidget *KalmanFilter::getWidget()
{
    return &widget;
}

void KalmanFilter::addPositive(cv::Mat image)
{
    // unused
}

void KalmanFilter::addNegative(cv::Mat image)
{
    // unused
}

void KalmanFilter::setFrame(AnnotatorLib::Frame *frame, cv::Mat image)
{
    this->lastFrame = this->frame;
    this->frame = frame;
    this->frameImg = image;
}


// first call
void KalmanFilter::setObject(AnnotatorLib::Object *object)
{
    this->object = object;
}

AnnotatorLib::Object *KalmanFilter::getObject()
{
    return object;
}

// second call
void KalmanFilter::setLastAnnotation(AnnotatorLib::Annotation *annotation)
{
    if(annotation == nullptr || annotation->getObject() != object)
        return;
    if(lastAnnotation != nullptr &&annotation->getObject() == lastAnnotation->getObject())
        return;
    this->lastAnnotation = annotation;
}

std::vector<AnnotatorLib::Commands::Command *> KalmanFilter::getCommands()
{
    std::vector<AnnotatorLib::Commands::Command *> commands;
    return commands;
}


//get annotation from kalmanfilter
AnnotatorLib::Annotation *KalmanFilter::getAnnotation()
{
    AnnotatorLib::Annotation * annotation = nullptr;
    return annotation;
}

void KalmanFilter::setSession(AnnotatorLib::Session *session)
{
    this->session = session;
}
