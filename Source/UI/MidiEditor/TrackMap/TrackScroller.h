/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class MidiRoll;
class TrackMap;
class TrackScrollerScreen;
class TransportIndicator;
class Transport;

#include "HelperRectangle.h"
#include "MidiRollListener.h"
#include "ComponentFader.h"

class TrackScroller :
    public Component,
    public MidiRollListener,
    private AsyncUpdater
{
public:

    TrackScroller(Transport &transport,
                  MidiRoll &roll);

    ~TrackScroller() override;
    
    void addOwnedMap(Component *newTrackMap, bool shouldBringToFront);

    void removeOwnedMap(Component *existingTrackMap);
    
    template<typename T>
    T *findOwnedMapOfType()
    {
        for (int i = 0; i < this->trackMaps.size(); ++i)
        {
            if (T *target = dynamic_cast<T *>(this->trackMaps.getUnchecked(i)))
            {
                return target;
            }
        }
        
        return nullptr;
    }


    //===------------------------------------------------------------------===//
    // TrackScroller
    //===------------------------------------------------------------------===//
    
    void xyMoveByUser();

    void xMoveByUser();

    void resizeByUser();
    
    void toggleStretchingMapAlaSublime();
    
    
    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void resized() override;

    void paintOverChildren(Graphics &g) override;
    
    void mouseDown(const MouseEvent &event) override;

    void mouseDrag(const MouseEvent &event) override;
    
    void mouseUp(const MouseEvent &event) override;
    
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;

    
    //===------------------------------------------------------------------===//
    // MidiRollListener
    //===------------------------------------------------------------------===//
    
    void onMidiRollMoved(MidiRoll *targetRoll) override;
    
    void onMidiRollResized(MidiRoll *targetRoll) override;
    
    
    //===------------------------------------------------------------------===//
    // Additional horizontal dragger
    //===------------------------------------------------------------------===//
    
    class HorizontalDragHelper : public HelperRectangle
    {
    public:
        
        explicit HorizontalDragHelper(TrackScroller &scrollerRef) :
            scroller(scrollerRef)
        {
            this->setInterceptsMouseClicks(true, false);
            this->toBack();
            
            this->moveConstrainer = new MoveConstrainer(this->scroller);
            this->moveConstrainer->setMinimumSize(4, 4);
            this->moveConstrainer->setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);
        }
        
        void mouseDown(const MouseEvent &e) override
        {
            this->dragger.startDraggingComponent(this, e);
        }
        
        void mouseDrag(const MouseEvent &e) override
        {
            this->dragger.dragComponent(this, e, this->moveConstrainer);
        }
        
        void paint(Graphics &g) override
        {
            g.setColour(Colours::white.withAlpha(0.02f));
            g.fillAll();
            
            //g.setColour(Colours::black.withAlpha(0.1f));
            //g.drawVerticalLine(0, 0.f, float(this->getHeight()));
            //g.drawVerticalLine(this->getWidth() - 1, 0.f, float(this->getHeight()));
        }
        
        class MoveConstrainer : public ComponentBoundsConstrainer
        {
        public:
            
            explicit MoveConstrainer(TrackScroller &scrollerRef) : scroller(scrollerRef) {}
            
            void applyBoundsToComponent(Component *component,
                                                const Rectangle<int> &bounds) override;
        private:
            
            TrackScroller &scroller;
        };
        
    private:
        
        TrackScroller &scroller;
        
        ComponentDragger dragger;
        
        ScopedPointer<ComponentBoundsConstrainer> moveConstrainer;
    };
    
protected:

    void horizontalDragByUser(Component *component, const Rectangle<int> &bounds);
    
    friend class HorizontalDragHelper;
    
private:
    
    void handleAsyncUpdate() override;
    
    Transport &transport;
    MidiRoll &roll;
    
    ScopedPointer<Component> background;
    ScopedPointer<TrackScrollerScreen> screenRange;
    ScopedPointer<TransportIndicator> indicator;
    
    OwnedArray<Component> trackMaps;

    void disconnectIndicator();
    Rectangle<float> getIndicatorBounds() const;
    Rectangle<int> getMapBounds() const;
    
    ComponentFader fader;
    ComponentDragger helperDragger;
    ScopedPointer<HelperRectangle> helperRectangle;
    
    bool mapShouldGetStretched;
    
};
