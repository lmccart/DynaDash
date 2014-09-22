//
//  AudioInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "AudioInput.h"
#include "StackedPlot.h"

struct ofSoundDevice {
    unsigned int index;
    string name;
    unsigned int outputChannels;
    unsigned int inputChannels;
    vector<unsigned int> sampleRates;
};

#include "RtAudio.h"
vector<ofSoundDevice> getDeviceList() {
    vector<ofSoundDevice> deviceList;
    RtAudio audio;
    int n = audio.getDeviceCount();
    for (int i = 0; i < n; i++) {
        RtAudio::DeviceInfo info = audio.getDeviceInfo(i);
        ofSoundDevice device;
        device.index = i;
        device.name = info.name;
        device.outputChannels = info.outputChannels;
        device.inputChannels = info.inputChannels;
        device.sampleRates = info.sampleRates;
        deviceList.push_back(device);
    }
    return deviceList;
}

vector<ofSoundDevice> findMatchingDevices(const vector<ofSoundDevice>& deviceList, string name) {
    vector<ofSoundDevice> matchingDevices;
    for(int i = 0; i < deviceList.size(); i++) {
        const ofSoundDevice& device = deviceList[i];
        if(device.name.find(name) != string::npos) {
            matchingDevices.push_back(device);
        }
    }
    return matchingDevices;
}

bool deviceComparator(const ofSoundDevice& a, const ofSoundDevice& b) {
    return a.name < b.name;
}

void sortDeviceList(vector<ofSoundDevice>& deviceList) {
    ofSort(deviceList, deviceComparator);
}

void AudioInput::setup() {   
    volume = vector<float>(4, 0);
    normalizedVolume = vector<float>(4, 0);
	speaking = vector<bool>(4, false);
	interrupting = vector<bool>(4, false);

	vector<ofSoundDevice> devices = getDeviceList();

	vector<ofSoundDevice> microphones = findMatchingDevices(devices, "C-Media USB Audio Device");
	
	if(microphones.size() == 0) {
        microphones = findMatchingDevices(devices, "Built-in Microphone");
	}
    
    if(microphones.size() != 4) {
        ofLogError() << "Detected " << microphones.size() << " microphones instead of 4.";
    }
    
    sortDeviceList(microphones);

	mics = vector<AudioAnalyzer>(4);
    for(int i = 0; i < microphones.size(); i++) {
		const ofSoundDevice& cur = microphones[i];
		ofLog() << "Connecting to " << cur.name << " at index " << cur.index << " " << i;
		mics[i].setup(cur.index);
    }
}

void AudioInput::setSmoothing(float amt) {
	AudioAnalyzer::smoothAmount = amt;
}

void AudioInput::update() {
    for (int i=0; i<4; i++) {
        volume[i] = mics[i].getVolume();
    }
	normalizedVolume = StackedPlot::normalize(volume);
	analyzeSpeaking();
}


void AudioInput::analyzeSpeaking() {
	int maxVolumeIndex = 0;
	float maxVolume = 0;
	for(int i = 0; i < 4; i++) {
		if(normalizedVolume[i] > maxVolume) {
			maxVolumeIndex = i;
			maxVolume = normalizedVolume[i];
		}
	}
	
	int lastSpeaker = -1;
	for (int i=0; i<4; i++) {
		if (speaking[i]) {
			lastSpeaker = i;
		}
	}

	for (int i=0; i<4; i++) {
		interrupting[i] = false;
		if (maxVolume > speakingNormalizedThresh && i == maxVolumeIndex) {
			speaking[i] = true;
			if (lastSpeaker != -1 && i != lastSpeaker) {
				interrupting[i] = true;
			}
		} else {
			speaking[i] = false;
		}
	}
}