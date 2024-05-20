/*
  ==============================================================================

    EnvelopeView.cpp
    Created: 17 May 2024 9:15:34pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EnvelopeView.h"

//==============================================================================


EnvelopeView::EnvelopeView(juce::RangedAudioParameter& param1Y,
                           juce::RangedAudioParameter& param2X,
                           juce::RangedAudioParameter& param2Y,
                           juce::RangedAudioParameter& param3X,
                           juce::RangedAudioParameter& param3Y,
                           juce::RangedAudioParameter& param4X,
                           juce::RangedAudioParameter& param4Y,
                           juce::UndoManager* um)
: audioParam1Y(param1Y)
, audioParam2X(param2X)
, audioParam2Y(param2Y)
, audioParam3X(param3X)
, audioParam3Y(param3Y)
, audioParam4X(param4X)
, audioParam4Y(param4Y)
, paramAttachment1Y(param1Y, [&] (float val) {updateNode1Y(val);}, um)
, paramAttachment2X(param2X, [&] (float val) {updateNode2X(val);}, um)
, paramAttachment2Y(param2Y, [&] (float val) {updateNode2Y(val);}, um)
, paramAttachment3X(param3X, [&] (float val) {updateNode3X(val);}, um)
, paramAttachment3Y(param3Y, [&] (float val) {updateNode3Y(val);}, um)
, paramAttachment4X(param4X, [&] (float val) {updateNode4X(val);}, um)
, paramAttachment4Y(param4Y, [&] (float val) {updateNode4Y(val);}, um)
, node1(nullptr, &audioParam1Y, nullptr, &paramAttachment1Y)
, node2(&audioParam2X, &audioParam2Y, &paramAttachment2X, &paramAttachment2Y)
, node3(&audioParam3X, &audioParam3Y, &paramAttachment3X, &paramAttachment3Y)
, node4(&audioParam4X, &audioParam4Y, &paramAttachment4X, &paramAttachment4Y)
{
    setName("View");
    
    addAndMakeVisible(node1);
    addAndMakeVisible(node2);
    addAndMakeVisible(node3);
    addAndMakeVisible(node4);
    
    node1.setComponentID("node1");
    node2.setComponentID("node2");
    node3.setComponentID("node3");
    node4.setComponentID("node4");

    node1.addMouseListener(this, false);
    node2.addMouseListener(this, false);
    node3.addMouseListener(this, false);
    node4.addMouseListener(this, false);
    
    paramAttachment1Y.sendInitialUpdate();
    paramAttachment2X.sendInitialUpdate();
    paramAttachment2Y.sendInitialUpdate();
    paramAttachment3X.sendInitialUpdate();
    paramAttachment3Y.sendInitialUpdate();
    paramAttachment4X.sendInitialUpdate();
    paramAttachment4Y.sendInitialUpdate();
}

EnvelopeView::~EnvelopeView()
{
    node1.removeMouseListener(this);
    node2.removeMouseListener(this);
    node3.removeMouseListener(this);
    node4.removeMouseListener(this);
}

void EnvelopeView::mouseDown(const juce::MouseEvent &event)
{
    const auto comp = event.originalComponent;
    if (comp->getTitle() == "node")
    {
        auto node = getNode(comp->getComponentID());
        
        if (node->paramAttachmentX)
            node->paramAttachmentX->beginGesture();
        node->paramAttachmentY->beginGesture();
        
        node->dragStartPos = node->pos;
    }
}

void EnvelopeView::mouseDrag(const juce::MouseEvent &event)
{
    const auto comp = event.originalComponent;
    if (comp->getTitle() == "node")
    {
        auto node = getNode(comp->getComponentID());
        
        if (node->paramAttachmentX)
        {
            const float normValueUnlimitedX = node->dragStartPos.x + event.getDistanceFromDragStartX() / timeParameterWidth;
            const float normValueX = juce::jlimit(0.0f, 1.0f, normValueUnlimitedX);
            const float denormValueX = node->audioParamX->convertFrom0to1(normValueX);
            node->paramAttachmentX->setValueAsPartOfGesture(denormValueX);
        }
        
        const float normValueUnlimitedY = node->dragStartPos.y - event.getDistanceFromDragStartY() / mainArea.getHeight();
        const float normValueY = juce::jlimit(0.0f, 1.0f, normValueUnlimitedY);
        const float denormValueY = node->audioParamY->convertFrom0to1(normValueY);
        node->paramAttachmentY->setValueAsPartOfGesture(denormValueY);
    }
}

void EnvelopeView::mouseUp(const juce::MouseEvent &event)
{
    const auto comp = event.originalComponent;
    if (comp->getTitle() == "node")
    {
        auto node = getNode(comp->getComponentID());
        
        if (node->paramAttachmentX)
            node->paramAttachmentX->endGesture();
        node->paramAttachmentY->endGesture();
    }
}

void EnvelopeView::paint (juce::Graphics& g)
{
    g.fillAll(MyColors::slate);
    
    juce::Path gridLines;
    
    for (int i = 0; i < numXGridLines; i++)
    {
        const float _x = mainArea.getX() + (static_cast<float>(i) / (numXGridLines - 1)) * mainArea.getWidth();
        
        gridLines.startNewSubPath(_x, bounds.getY());
        gridLines.lineTo         (_x, bounds.getBottom());
    }
    
    for (int i = 0; i < numYGridLines; i++)
    {
        const float _y = mainArea.getY() + (static_cast<float>(i) / (numYGridLines - 1)) * mainArea.getHeight();
        
        gridLines.startNewSubPath(bounds.getX(), _y);
        gridLines.lineTo         (bounds.getRight(), _y);
    }
    
    g.setColour(MyColors::darkGreen);
    g.strokePath(gridLines, juce::PathStrokeType(gridLineWidth));
    
    juce::Path envGraph;
    envGraph.startNewSubPath(node1.getPosition().toFloat());
    envGraph.lineTo(node2.getPosition().toFloat());
    envGraph.lineTo(node3.getPosition().toFloat());
    envGraph.lineTo(node4.getPosition().toFloat());
    
    g.setColour(MyColors::white);
    g.strokePath(envGraph, juce::PathStrokeType(gridLineWidth));
    
    g.setColour(MyColors::lightGrey);
    g.drawRect(bounds);
}

void EnvelopeView::resized()
{
    bounds = getLocalBounds().toFloat();
    mainArea = bounds.reduced(6.0f);
    
    timeParameterWidth = mainArea.getWidth() * 0.3f;
    
    positionNodes();
}

void EnvelopeView::positionNodes()
{
    const juce::Rectangle<float> nodeBounds (nodeSize, nodeSize);
    
    node1.setBounds(nodeBounds.withX(mainArea.getX())
                    .withY(mainArea.getBottom() - node1.pos.y * mainArea.getHeight())
                    .toNearestInt());
    node2.setBounds(nodeBounds.withX(mainArea.getX() + node2.pos.x * timeParameterWidth)
                    .withY(mainArea.getBottom() - node2.pos.y * mainArea.getHeight())
                    .toNearestInt());
    node3.setBounds(nodeBounds.withX(node2.getX() + node3.pos.x * timeParameterWidth)
                    .withY(mainArea.getBottom() - node3.pos.y * mainArea.getHeight())
                    .toNearestInt());
    node4.setBounds(nodeBounds.withX(node3.getX() + node4.pos.x * timeParameterWidth)
                    .withY(mainArea.getBottom() - node4.pos.y * mainArea.getHeight())
                    .toNearestInt());
    
    repaint();
}

EnvelopeNode* EnvelopeView::getNode(const juce::String& id)
{
    if (id == "node1")
        return &node1;
    else if (id == "node2")
        return &node2;
    else if (id == "node3")
        return &node3;
    else
        return &node4;
}

void EnvelopeView::updateNode1Y(float newVal)
{
    node1.pos.y = audioParam1Y.convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeView::updateNode2X(float newVal)
{
    node2.pos.x = audioParam2X.convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeView::updateNode2Y(float newVal)
{
    node2.pos.y = audioParam2Y.convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeView::updateNode3X(float newVal)
{
    node3.pos.x = audioParam3X.convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeView::updateNode3Y(float newVal)
{
    node3.pos.y = audioParam3Y.convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeView::updateNode4X(float newVal)
{
    node4.pos.x = audioParam4X.convertTo0to1(newVal);
    positionNodes();
}

void EnvelopeView::updateNode4Y(float newVal)
{
    node4.pos.y = audioParam4Y.convertTo0to1(newVal);
    positionNodes();
}
