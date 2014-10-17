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

vector<ofSoundDevice> findMatchingDevices(const vector<ofSoundDevice>& deviceList, string name, int inputChannels, int outputChannels) {
    vector<ofSoundDevice> matchingDevices;
    for(int i = 0; i < deviceList.size(); i++) {
        const ofSoundDevice& device = deviceList[i];
        if(device.name.find(name) != string::npos &&
           device.inputChannels == inputChannels &&
           device.outputChannels == outputChannels) {
            matchingDevices.push_back(device);
        } //else ofLogNotice() << device.inputChannels << " " << device.outputChannels << " " << device.name;
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
    curSpeaker = -1;

	vector<ofSoundDevice> devices = getDeviceList();

	vector<ofSoundDevice> microphones = findMatchingDevices(devices, "C-Media USB Audio Device", 2, 0);
	
	if(microphones.size() == 0) {
        microphones = findMatchingDevices(devices, "Built-in Microphone", 2, 0);
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
    micsInited = microphones.size();
}

void AudioInput::setSmoothing(float amt) {
	AudioAnalyzer::smoothAmount = amt;
}

void AudioInput::update() {
    for (int i=0; i<micsInited; i++) {
        volume[i] = mics[i].getVolume();
    }
	normalizedVolume = StackedPlot::normalize(volume);
	analyzeSpeaking();
}


void AudioInput::analyzeSpeaking() {
	int maxVolumeIndex = 0;
	float maxVolume = 0;
	for(int i = 0; i < micsInited; i++) {
		if(normalizedVolume[i] > maxVolume) {
			maxVolumeIndex = i;
			maxVolume = normalizedVolume[i];
		}
	}
    
    curSpeaker = -1;
	for (int i=0; i<micsInited; i++) {
		if (speaking[i]) {
			curSpeaker = i;
		}
	}

	for (int i=0; i<micsInited; i++) {
		interrupting[i] = false;
		if (maxVolume > speakingNormalizedThresh && i == maxVolumeIndex) {
			speaking[i] = true;
			if (curSpeaker != -1 && i != curSpeaker) {
				interrupting[i] = true;
			}
		} else {
			speaking[i] = false;
		}
	}
}