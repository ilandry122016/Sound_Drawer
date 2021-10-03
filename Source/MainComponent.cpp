#include "MainComponent.h"

MainComponent::MainComponent(): image(Image::RGB, 1024, 400, true)
{
    image.clear(Rectangle<int> (image.getWidth(),image.getHeight()),
                Colour(255,255,255));
    // Make sure you set the size of the component after
    // you add any child components.
    double canvasX = 20;
    wtSize = 1024;
    setSize (wtSize + 2*canvasX, 600);
    waveTable.resize(wtSize);
    waveTable.fill(0);
    
    freqSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    freqSlider.setRange(50, 500);
    freqSlider.setTextValueSuffix("Hz");
    freqSlider.addListener(this);
    freqSlider.setValue(440);
    addAndMakeVisible(freqSlider);
    freqLabel.setText("Frequency", dontSendNotification);
    freqLabel.attachToComponent(&freqSlider, true);
    
    ampSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    ampSlider.setRange(0.0, 1.0);
    ampSlider.addListener(this);
    ampSlider.setValue(0.3);
    addAndMakeVisible(ampSlider);
    ampLabel.setText("Amplitude", dontSendNotification);
    ampLabel.setColour(1, Colours::maroon);
    ampLabel.attachToComponent(&ampSlider, true);

    addAndMakeVisible(&sineButton);
    sineButton.setButtonText("Sine");

    sineButton.onClick = [this] {updateToggleState(&sineButton, "Sine");
        waveTable.clear();
        for (int i = 0; i < wtSize; i++) {
            waveTable.insert(i, sin((2.0 * double_Pi * i) / wtSize));
        }
        repaint();
    };
    
    addAndMakeVisible(&squareButton);
    squareButton.setButtonText("Square");
    squareButton.onClick = [this] {updateToggleState(&squareButton, "Square");
        waveTable.clear();
        for (int i = 0; i < wtSize/2; i++) {
            waveTable.insert(i, 1.0f);
        }
        for (int i = wtSize/2; i < wtSize; i++) {
            waveTable.insert(i, -1.0f);
        }
        repaint();
    };
    
    addAndMakeVisible(&sawtoothButton);
    sawtoothButton.setButtonText("Sawtooth");
    sawtoothButton.onClick = [this] {updateToggleState(&sawtoothButton, "Sawtooth");
        waveTable.clear();
        for (int i = 0; i < wtSize; i++) {
          waveTable.insert(i, (1.0 * i - wtSize/2) / (wtSize/2));
        }
        repaint();
    };
    
    addAndMakeVisible(&triangleButton);
    triangleButton.setButtonText("Triangle");
    triangleButton.onClick = [this] {updateToggleState(&triangleButton, "Triangle");
        waveTable.clear();
        for (int i = 0; i < wtSize/2; i++) {
          waveTable.insert(i, -1 + (2.0*i) / (wtSize/2));
        }
        for (int i = wtSize/2; i < wtSize; i++) {
          waveTable.insert(i, 1 - (2.0*(i-wtSize/2)) / (wtSize/2));
        }
        repaint();
    };
    
    sineButton.setRadioGroupId(WaveButtons);
    squareButton.setRadioGroupId(WaveButtons);
    sawtoothButton.setRadioGroupId(WaveButtons);
    triangleButton.setRadioGroupId(WaveButtons);
    
    setAudioChannels (0, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::releaseResources()
{
}
