#include "camshift.h"
#include "widget.h"

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Frame.h>
#include <AnnotatorLib/Commands/NewAnnotation.h>
#include <QtGui/QPainter>
#include <QDebug>
#include <opencv2/video/tracking.hpp>

//temp include ADIL
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <ctype.h>

using namespace Annotator::Plugins;

Annotator::Plugins::CamShift::CamShift()
{

    hranges[0] = 0.0;
    hranges[1] = 180.0;
    phranges = hranges;
}

CamShift::~CamShift()
{

}

QString CamShift::getName()
{
    return "CamShift";
}

QWidget *CamShift::getWidget()
{
    return &widget;
}

void CamShift::addPositive(cv::Mat image)
{
    // unused
}

void CamShift::addNegative(cv::Mat image)
{
    // unused
}

void CamShift::setFrame(AnnotatorLib::Frame *frame, cv::Mat image)
{
    this->lastFrame = this->frame;
    this->frame = frame;
    this->frameImg = image;
}


// first call
void CamShift::setObject(AnnotatorLib::Object *object)
{
    this->object = object;
}

AnnotatorLib::Object *CamShift::getObject()
{
    return object;
}

// second call
void CamShift::setLastAnnotation(AnnotatorLib::Annotation *annotation)
{
    if(annotation == nullptr || annotation->getObject() != object)
        return;
    if(lastAnnotation != nullptr &&annotation->getObject() == lastAnnotation->getObject())
        return;


    this->lastAnnotation = annotation;
    selection = cv::Rect(lastAnnotation->getX() - lastAnnotation->getHRadius(),
                            lastAnnotation->getY() - lastAnnotation->getVRadius(),
                            lastAnnotation->getHRadius() * 2, lastAnnotation->getVRadius() * 2);

    newSelection = true;
}

std::vector<AnnotatorLib::Commands::Command *> CamShift::getCommands()
{
    std::vector<AnnotatorLib::Commands::Command *> commands;
    if(object == nullptr || frame == nullptr || lastFrame == nullptr || this->lastAnnotation == nullptr)
        return commands;

    try{

    cv::Rect found_rect = findObject();

    int w = found_rect.width * 0.5;
    int h = found_rect.height * 0.5;

    int x = found_rect.x + w;
    int y = found_rect.y + h;


    AnnotatorLib::Commands::NewAnnotation * nA =
            new AnnotatorLib::Commands::NewAnnotation(lastAnnotation->getObject(), this->frame,
                                                      x, y, w, h,
                                                      lastAnnotation->getNext(), lastAnnotation->isInterpolated() ? lastAnnotation->getPrevious() : lastAnnotation,
                                                      this->session, false);
    commands.push_back(nA);

    }catch(std::exception &e){
        qDebug() << e.what();
    }

    return commands;
}


//get annotation from camshift
AnnotatorLib::Annotation *CamShift::getAnnotation()
{
    if(object == nullptr || frame == nullptr || lastFrame == nullptr || this->lastAnnotation == nullptr)
        return nullptr;

    AnnotatorLib::Annotation * annotation = nullptr;
    /*
    try{

    cv::Rect found_rect = findObject();

    annotation = new AnnotatorLib::Annotation(lastAnnotation->getType());
    annotation->setPrevious(lastAnnotation);
    annotation->setObject(lastAnnotation->getObject());
    annotation->setFrame(this->frame);
    annotation->setPosition(found_rect.x + found_rect.width * 0.5 , found_rect.y + found_rect.height * 0.5,
                              found_rect.width * 0.5, found_rect.height * 0.5);
    annotation->setInterpolated(false);
    }catch(cv::Exception &e){
        qDebug() << e.what();
    }
    */
    return annotation;
}

void CamShift::setSession(AnnotatorLib::Session *session)
{
    this->session = session;
}



cv::Rect CamShift::findObject()
{

    cv::cvtColor(frameImg, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, cv::Scalar(0, smin, std::min(vmin, vmax)),
    cv::Scalar(180, 256, std::max(vmin, vmax)), mask);

    int ch[2] = { 0,0 };

    hue.create(hsv.size(), hsv.depth());
    cv::mixChannels(&hsv, 1, &hue, 1, ch, 1);

    if(newSelection){
        selection &= cv::Rect(0, 0, frameImg.cols, frameImg.rows);
        cv::Mat roi(hue, selection), maskroi(mask, selection);
        cv::calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
        cv::normalize(hist, hist, 0, 255, CV_MINMAX);
        trackWindow = selection;
        newSelection = false;
    }

    cv::calcBackProject(&hue, 1, 0, hist, backproj, &phranges);

    backproj &= mask;

    cv::TermCriteria criteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 10, 1);

    cv::RotatedRect found_object = cv::CamShift(backproj, trackWindow, criteria);


    // debug
    cv::ellipse( frameImg, found_object, cv::Scalar(0,0,255), 3, CV_AA );
    //cv::rectangle(frameImg, found_object.boundingRect(), cv::Scalar(0,0,255), 3, CV_AA);
    cv::imshow( "CamShift Debug", frameImg );

    return found_object.boundingRect();
}

