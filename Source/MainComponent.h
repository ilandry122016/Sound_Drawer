#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <iostream>

class MainComponent   : public AudioAppComponent, public Slider::Listener
{
public:
    
    enum RadioButtonIds
    {
        WaveButtons = 1001
    };
    
    void updateToggleState (Button* button, String name)
    {
        auto state = button->getToggleState();
        String stateString    = state ? "ON" : "OFF";
        String selectedString = state ? " (selected)" : "";

        Logger::outputDebugString (name + " Button changed to " + stateString);
        button->setButtonText (name + selectedString);
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (slider == &freqSlider)
        {
            frequency = freqSlider.getValue();
        } else if (slider == &ampSlider)
        {
            amplitude = ampSlider.getValue();
        } else if (slider == &coSlider)
        {
            coeff = coSlider.getValue();
        }
        repaint();
    }
    
    void updateFrequency()
    {
      phase+= (frequency * wtSize) / currentSampleRate;
    }
    
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        frequency = freqSlider.getValue();
        phase = 0;
        amplitude = ampSlider.getValue();
        currentSampleRate = sampleRate;
        coeff = coSlider.getValue();
    }
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {

        float* const leftSpeaker = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        float* const rightSpeaker = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        bufferToFill.clearActiveBufferRegion();
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            const int int_phase(static_cast<int>(phase)%wtSize);
            leftSpeaker[sample] += waveTable[int_phase] * amplitude;
            rightSpeaker[sample] += waveTable[int_phase] * amplitude;
            updateFrequency();
        }
        
    }
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override
    {
        double canvasX = 20;
        double canvasY = 100;
        double canvasSpaceX = getWidth() - 2*canvasX;
        double canvasSpaceY = getHeight() - 2*canvasY;
        g.fillAll(Colour (0xffffd4ff));
        g.drawRect(20, 100, canvasSpaceX, canvasSpaceY);
        g.setOpacity(1.0);
        image.clear(Rectangle<int> (image.getWidth(),image.getHeight()),
                    Colour(255,255,255));
        Graphics w(image);
        for(int i=1; i< wtSize; i++) {
          w.drawLine(i-1, canvasSpaceY - (canvasSpaceY/2)*(amplitude*waveTable[i-1] + 1),
                     i,canvasSpaceY - (canvasSpaceY/2)*(amplitude*waveTable[i] + 1));
        }
        g.drawImageAt(image,canvasX,canvasY);
    };
    
    void resized() override
    {
        const int labelSpace = 100;
        freqSlider.setBounds(labelSpace, 20, getWidth() - 100, 20);
        ampSlider.setBounds(labelSpace, 50, getWidth() - 100, 50);
        sineButton.setBounds(0, getHeight() - 90, getWidth()/4, 20);
        squareButton.setBounds(getWidth()/4, getHeight() - 90, getWidth()/4, 20);
        sawtoothButton.setBounds(getWidth()/2, getHeight() - 90, getWidth()/4, 20);
        triangleButton.setBounds(3*getWidth()/4, getHeight() - 90, getWidth()/4, 20);
    };
    
    void mouseMove(const MouseEvent& event) override
    {
        m_mousepos = event.position;
        repaint();
    }
    
    void mouseDown(const MouseEvent &event) override
    {
        double canvasX = 20;
        double canvasY = 100;
        double canvasSpaceX = getWidth() - 2*canvasX;
        double canvasSpaceY = getHeight() - 2*canvasY;
        is_pressed = true;
        m_mousepos = event.position;
        repaint();
    }
    
    void mouseDrag(const MouseEvent &event) override
    {
        double canvasX = 20;
        double canvasY = 100;
        double canvasSpaceX = getWidth() - 2*canvasX;
        double canvasSpaceY = getHeight() - 2*canvasY;
        is_pressed = true;
        Graphics w(image);
        auto new_pos(event.position);

        int new_x = new_pos.getX() - canvasX;
        int new_y = new_pos.getY() - canvasY;

        if(new_x < 0)
          {
            new_x = 0;
          }
        if(new_x >= wtSize)
          {
            new_x = wtSize-1;
          }
        if(new_y < 0)
          {
            new_y = 0;
          }
        if(new_y >= canvasSpaceY)
          {
            new_y = canvasSpaceY-1;
          }

        int old_x = m_mousepos.getX() - canvasX;
        int old_y = m_mousepos.getY() - canvasY;

        if(old_x < 0)
          {
            old_x = 0;
          }
        if(old_x >= wtSize)
          {
            old_x = wtSize-1;
          }
        if(old_y < 0)
          {
            old_y = 0;
          }
        if(old_y >= canvasSpaceY)
          {
            old_y = canvasSpaceY-1;
          }
        
        int increment = new_x > old_x ? 1 : -1;
        for(int x=old_x; x!= new_x; x+=increment)
          {
            double fraction = (x - old_x) / (new_x - old_x);
            waveTable.set(x, (1 - old_y/(canvasSpaceY/2)) * (1-fraction)
                          + (1 - new_y/(canvasSpaceY/2)) * fraction);

          }
        m_mousepos=new_pos;
        repaint();
    }
    
    void mouseUp(const MouseEvent &event) override
    {
        is_pressed = false;

    }
        
    void mouseEnter(const MouseEvent& event) override
    {
        is_entered = true;
    }
    
    void mouseExit (const MouseEvent& event) override
    {
        is_entered = false;
    }

private:
    Slider freqSlider, ampSlider, coSlider;
    Label freqLabel, ampLabel, coLabel;

    ToggleButton sineButton;
    ToggleButton squareButton;
    ToggleButton sawtoothButton;
    ToggleButton triangleButton;
    
    Array<float> waveTable;
    
    int wtSize;
    double frequency;
    double phase;
    double amplitude;
    double currentSampleRate;
    double coeff;
    
    AudioTransportSource transportSource;

    juce::Point<float> m_mousepos;
    bool is_entered;
    bool is_pressed;

    Image image;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
