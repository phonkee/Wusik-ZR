// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

	Created by William Kalfelz @ Wusik Dot Com (c) 2019

*/

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "PluginProcessor.h"
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::loadWaveBanks(char bankSlot, String name, int Zone)
{
	if (dataFiles->dataFolder.isEmpty()) return;
	//
	if (bankSlot == 99)
	{
		for (int zones = 0; zones < 4; zones++)
		{
			loadWaveBanks(0, programs[currentProgram]->waveBankName[zones][0], zones);
			loadWaveBanks(1, programs[currentProgram]->waveBankName[zones][1], zones);
			loadWaveBanks(2, programs[currentProgram]->waveBankName[zones][2], zones);
			loadWaveBanks(3, programs[currentProgram]->waveBankName[zones][3], zones);
		}
	}
	else
	{
		if (Zone == -1) Zone = currentZone;
		//
		if (curWaveBankName[Zone][bankSlot] != name)
		{
			programs[currentProgram]->waveBankName[Zone][bankSlot] = name;
			curWaveBankName[Zone][bankSlot] = name;
			//
			File theFile(dataFiles->dataFolder + "Wusik ZR" + slash + "Wavebanks" + slash + name + ".WusikWavebank");
			if (theFile.existsAsFile())
			{
				ScopedPointer<FileInputStream> theStream = new FileInputStream(theFile);
				ScopedPointer<GZIPDecompressorInputStream> zipStream;
				//
				String theHeader = theStream->readString();
				if (theHeader.compare("WusikWavebank<2019>Wusik.com") != 0) goto xError;
				zipStream = new GZIPDecompressorInputStream(theStream, false);
				//
				{
					if (zipStream->readByte() != 'F') goto xError;
					int formatType = zipStream->readByte();
					if (zipStream->readByte() != 'W') goto xError;
					int nWaveforms = zipStream->readByte();
					//
					for (int xw = 0; xw < nWaveforms; xw++)
					{
						if (zipStream->readByte() != 'D') goto xError;
						zipStream->read(wavetables[Zone][xw + (bankSlot * 12)].wavetable, sizeof(float) * (WAVE_SAMPLE_LENGTH + 3));
						if (zipStream->readByte() != 'T') goto xError;
						wavetables[Zone][xw + (bankSlot * 12)].tune = zipStream->readByte();
						wavetables[Zone][xw + (bankSlot * 12)].fine = zipStream->readFloat();
						if (zipStream->readByte() != 'E') goto xError;
					}
					//
					if (zipStream->readByte() != 'X') goto xError;
				}
				//
				zipStream = nullptr;
				theStream = nullptr;
				//
				for (int xw = 0; xw < 12; xw++)
				{
					for (int xxx = 0; xxx < 10; xxx++)
					{
						wavetables[Zone][xw + (bankSlot * 12)].wavetable[1200 + xxx] = wavetables[Zone][xw + (bankSlot * 12)].wavetable[xxx];
					}
					//
					wavetables[Zone][xw + (bankSlot * 12)].waveform = Image(Image::PixelFormat::ARGB, 600, 600, true);
					Graphics gG(wavetables[Zone][xw + (bankSlot * 12)].waveform);
					gG.setColour(dataFiles->waveformColour);
					//
					int pos = 0;
					for (int xs = 0; xs < 600; xs++)
					{
						float* xWave = wavetables[Zone][xw + (bankSlot * 12)].wavetable;
						int xValue = xWave[pos] * 300.0f;
						gG.drawLine(xs, 300, xs, 300 - xValue);
						//
						pos += 2;
					}
				}
				//
				if (getActiveEditor() != nullptr)
				{
					getActiveEditor()->repaint();
					if (waveformWindow != nullptr) waveformWindow->repaint();
				}
				return;
				//
			xError:
				zipStream = nullptr;
				theStream = nullptr;
				//
				MessageBox("Error Loading Wavebank!", theFile.getFileNameWithoutExtension());
			}
			else
			{
				MessageBox("Missing Wavebank File!", theFile.getFileNameWithoutExtension());
			}
		}
	}
}
//
#if WCREATE_WAVE_BANKS
#define CHECK_WAVEFORMS_AND_SAVE if (slot != 12) MessageBox("Number of Waveforms for the bank is wrong: " + bankname, String(slot)); saveBinary("D:\\Wusik Engine Data\\Wusik ZR\\Wavebanks\\" + bankname + ".WusikWavebank", bankname)
#define START_BANK(name) for (int xs = 0; xs < 12; xs++) { wavetables[0][xs].tune = 0; wavetables[0][xs].fine = 0.0f; } slot = 0; bankname = name; waveformNames.clear()
#define WTEST_AUDIO_WRITEFILE 0

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::createWaveBanks()
{
	String bankname;
	int slot = 0;
	//
	START_BANK("Saws 1");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0001.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0002.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0005.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0007.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0011.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0013.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0016.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0043.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0050.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0019.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0035.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_saw\\AKWF_saw_0030.wav", slot); slot++; 
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Saws 2");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawbright\\AKWF_bsaw_0001.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawbright\\AKWF_bsaw_0004.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawbright\\AKWF_bsaw_0007.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawbright\\AKWF_bsaw_0009.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawbright\\AKWF_bsaw_0010.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawgap\\AKWF_gapsaw_0019.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawgap\\AKWF_gapsaw_0029.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawgap\\AKWF_gapsaw_0030.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawgap\\AKWF_gapsaw_0031.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_sawgap\\AKWF_gapsaw_0032.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_perfectwaves\\AKWF_saw.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bitreduced\\AKWF_saw8bit.wav", slot); slot++;		
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Game 1");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0001.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0002.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0003.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0004.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0005.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0006.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0007.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0008.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0009.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0010.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0011.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0012.wav", slot); slot++;	
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Game 2");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0013.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0014.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0015.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0016.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0017.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0018.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0019.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0020.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0021.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0022.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0023.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0024.wav", slot); slot++;	
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Game 3");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0025.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0026.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0027.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0028.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0029.wav", slot); slot++; 
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0030.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0031.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_c604\\AKWF_c604_0032.wav", slot); slot++;	
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bitreduced\\AKWF_saw6bit.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bitreduced\\AKWF_sin3bit.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bitreduced\\AKWF_tri4bit.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bitreduced\\AKWF_bitreduced_0022.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Spectrum");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 1.Flac", slot, 200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 2.Flac", slot, 200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 3.Flac", slot, 200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 4.Flac", slot, 200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 5.Flac", slot, 200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 6.Flac", slot, 200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 7.Flac", slot, 200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 1.Flac", slot, 2000, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 1.Flac", slot, 8000, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 3.Flac", slot, 5000, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 7.Flac", slot, 5200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Spect 7.Flac", slot, 9200, true); slot++;
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Vox 1");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Vox Z.Flac", slot, 1200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Vox 4.Flac", slot, 1200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Vox 3.Flac", slot, 1200, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\Retro DS\\Vox Z.Flac", slot, 25000, true); slot++;
	loadWave("D:\\My Samples\\Wusik 3rd Party Sounds\\Artvera MISTRAL\\VK-voice1-NL.Flac", slot, 12000, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\ElektroWusik\\Vocals\\Vocoded\\Aktivitak 1.Flac", slot, 1400, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\ElektroWusik\\Vocals\\Phrases Dry\\Darla Phrases 001.Flac", slot, 36000, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\ElektroWusik\\Vocals\\Phrases Dry\\Darla Phrases 002.Flac", slot, 36000, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\ElektroWusik\\Vocals\\Phrases Dry\\Darla Phrases 003.Flac", slot, 36000, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\ElektroWusik\\Vocals\\Phrases Dry\\Darla Phrases 004.Flac", slot, 36000, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\ElektroWusik\\Vocals\\Phrases Dry\\Darla Phrases 005.Flac", slot, 64000, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\ElektroWusik\\Vocals\\Phrases Dry\\Darla Phrases 006.Flac", slot, 65000, true); slot++;
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Vox 2");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0002.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0004.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0008.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0011.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0012.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0014.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0015.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0018.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0019.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0020.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0021.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0028.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Vox 3");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0030.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0035.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0037.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0038.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0040.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0041.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0044.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0045.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0050.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0060.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0096.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hvoice\\AKWF_hvoice_0098.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Vox 4");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\FUNCNORP.Flac", slot, 20, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\FUNCNORP.Flac", slot, 11052, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\FUNCNORP.Flac", slot, 38600, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\FUNCNORP.Flac", slot, 45651, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\FUNCNORP.Flac", slot, 84015, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\FUNCNORP.Flac", slot, 101795, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\CMONDOWN.Flac", slot, 21258, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\CMONDOWN.Flac", slot, 43591, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\ATTNTION.Flac", slot, 14673, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\ATTNTION.Flac", slot, 2353, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\ATTNTION.Flac", slot, 34468, true, true); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\V3 Test Sounds\\ATTNTION.Flac", slot, 58509, true, true); slot++;
	CHECK_WAVEFORMS_AND_SAVE;
	//
	START_BANK("Basic");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_perfectwaves\\AKWF_saw.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_perfectwaves\\AKWF_squ.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_perfectwaves\\AKWF_sin.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_perfectwaves\\AKWF_tri.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hdrawn\\AKWF_hdrawn_0004.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hdrawn\\AKWF_hdrawn_0001.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hdrawn\\AKWF_hdrawn_0002.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hdrawn\\AKWF_hdrawn_0005.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hdrawn\\AKWF_hdrawn_0006.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hdrawn\\AKWF_hdrawn_0008.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hdrawn\\AKWF_hdrawn_0020.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_hdrawn\\AKWF_hdrawn_0026.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Granular 1");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0001.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0002.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0003.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0004.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0005.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0006.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0007.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0008.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0009.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0010.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0011.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0012.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Granular 2");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0013.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0014.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0015.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0016.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0017.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0018.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0019.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0020.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0021.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0022.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0023.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0024.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Granular 3");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0025.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0026.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0027.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0028.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0029.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0030.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0031.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0032.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0033.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0034.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0035.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_granular\\AKWF_granular_0036.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Noise");
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise1.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise2.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise3.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise4.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise5.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise6.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise7.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise8.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise9.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise10.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise11.wav", slot, 40, true, true); slot++;
	loadWave("D:\\Work Ready\\Wusik ZR\\Waveforms\\Noise12.wav", slot, 40, true, true); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("FM 1");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0001.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0002.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0003.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0004.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0005.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0006.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0007.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0008.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0009.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0010.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0011.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0012.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("FM 2");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0013.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0014.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0015.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0016.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0017.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0018.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0019.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0020.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0021.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0022.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0023.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0024.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("FM 3");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0025.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0026.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0027.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0028.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0029.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0030.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0031.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0032.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0033.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0034.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0035.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0036.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("FM 4");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0037.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0038.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0039.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0040.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0041.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0042.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0043.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0044.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0045.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0046.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0047.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_fmsynth\\AKWF_fmsynth_0048.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Mixed 1");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0001\\AKWF_0001.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0001\\AKWF_0002.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0002\\AKWF_0101.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0002\\AKWF_0102.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0003\\AKWF_0201.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0003\\AKWF_0202.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0004\\AKWF_0301.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0004\\AKWF_0302.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0005\\AKWF_0401.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0005\\AKWF_0402.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0006\\AKWF_0501.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0006\\AKWF_0502.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Mixed 2");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0001\\AKWF_0005.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0001\\AKWF_0006.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0002\\AKWF_0105.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0002\\AKWF_0106.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0003\\AKWF_0207.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0003\\AKWF_0208.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0004\\AKWF_0306.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0004\\AKWF_0304.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0005\\AKWF_0409.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0005\\AKWF_0408.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0006\\AKWF_0507.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0006\\AKWF_0505.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Mixed 3");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0007\\AKWF_0601.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0007\\AKWF_0602.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0008\\AKWF_0701.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0008\\AKWF_0702.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0009\\AKWF_0801.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0009\\AKWF_0802.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0010\\AKWF_0901.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0010\\AKWF_0902.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0011\\AKWF_1001.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0011\\AKWF_1002.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0012\\AKWF_1101.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0012\\AKWF_1102.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Mixed 4");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0007\\AKWF_0605.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0007\\AKWF_0606.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0008\\AKWF_0705.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0008\\AKWF_0706.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0009\\AKWF_0807.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0009\\AKWF_0808.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0010\\AKWF_0906.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0010\\AKWF_0904.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0011\\AKWF_1009.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0011\\AKWF_1008.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0012\\AKWF_1107.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0012\\AKWF_1105.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Mixed 5");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0007\\AKWF_0612.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0007\\AKWF_0611.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0008\\AKWF_0712.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0008\\AKWF_0711.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0009\\AKWF_0812.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0009\\AKWF_0811.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0010\\AKWF_0912.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0010\\AKWF_0911.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0011\\AKWF_1012.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0011\\AKWF_1011.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0012\\AKWF_1112.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_0012\\AKWF_1111.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Mixed 6");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0001.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0002.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0003.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0004.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0005.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0006.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0007.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0008.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0009.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0070.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0011.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0012.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Mixed 7");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0013.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0014.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0015.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0016.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0017.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0018.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0019.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0071.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0021.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0022.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0073.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0077.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Mixed 8");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0025.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0026.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0055.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0028.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0029.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0030.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0031.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0032.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0033.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0056.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0035.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_bw_blended\\AKWF_blended_0036.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Harmonics");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0001.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0002.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0003.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0004.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0005.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0006.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0007.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0008.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0009.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0010.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0011.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_sinharm\\AKWF_sinharm_0012.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Distorted 1");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0001.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0002.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0003.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0004.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0005.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0006.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0007.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0008.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0009.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0010.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0011.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0012.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Distorted 2");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0013.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0014.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0015.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0016.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0017.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0018.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0019.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0020.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0021.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0022.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0023.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0024.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;

	START_BANK("Distorted 3");
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0025.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0026.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0027.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0028.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0029.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0030.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0031.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0032.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0033.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0034.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0035.wav", slot); slot++;
	loadWave("D:\\Wusik Engine Data\\Shared\\SoundSets\\www.AdventureKid.se\\AKWF_distorted\\AKWF_distorted_0036.wav", slot); slot++;
	CHECK_WAVEFORMS_AND_SAVE;
}
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::saveBinary(String filename, String name)
{
	File output(filename);
	output.createDirectory();
	output.deleteFile();
	//
	ScopedPointer<FileOutputStream> xStream = output.createOutputStream();
	//
	xStream->writeString("WusikWavebank<2019>Wusik.com");
	xStream->flush();
	//
	ScopedPointer<GZIPCompressorOutputStream> zipStream = new GZIPCompressorOutputStream(xStream);
	//
	zipStream->writeByte('F');
	zipStream->writeByte(1); //  Format 1 //
	zipStream->writeByte('W');
	zipStream->writeByte(12); // Total Waveforms //
	//
	for (int xw = 0; xw < 12; xw++)
	{
		zipStream->writeByte('D');
		zipStream->write(wavetables[0][xw].wavetable, sizeof(float) * (WAVE_SAMPLE_LENGTH + 3));
		zipStream->writeByte('T');
		zipStream->writeByte(wavetables[0][xw].tune);
		zipStream->writeFloat(wavetables[0][xw].fine);
		zipStream->writeByte('E');
	}
	//
	zipStream->writeByte('X'); // Bank End //
	//
	zipStream->flush();
	xStream->flush();
	zipStream = nullptr;
	xStream = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::loadWave(String filename, int target, int offset, bool fade, bool applyLimiter)
{
	if (!File(filename).existsAsFile()) MessageBox("File Not Found!", filename);
	//
	AudioFormatManager formatManager;
	formatManager->registerBasicFormats();
	//
	ScopedPointer<AudioFormatReader> reader = formatManager->createReaderFor(filename);
	//
	if (reader != nullptr && reader->lengthInSamples > 0 && reader->numChannels > 0)
	{
		AudioSampleBuffer xbuffer(reader->numChannels, reader->lengthInSamples);
		reader->read(&xbuffer, 0, reader->lengthInSamples, 0, true, false);
		//
		/*float xRt = float(reader->lengthInSamples) / 1200.0f;
		if (xRt <= 0.0f) xRt = 1.0f;
		float xCt = 0.0f;
		for (int32 xx = 0; xx < 1200; xx++)
		{
			wavetables[target].wavetable[xx] = xbuffer.getWritePointer(0)[jlimit(0, int32(reader->lengthInSamples - 1), int32(xCt))];
			xCt += xRt;
		}*/
		//
		if (offset == 0)
		{
			int xCounter = 0;
			for (int32 xx = 0; xx < 600; xx++) // Adventure Kid's Waveforms are 600 samples //
			{
				if (xx < 599)
				{
					wavetables[0][target].wavetable[xCounter + 0] = xbuffer.getWritePointer(0)[xx];
					wavetables[0][target].wavetable[xCounter + 1] =
						xbuffer.getWritePointer(0)[xx] + ((xbuffer.getWritePointer(0)[xx + 1] - xbuffer.getWritePointer(0)[xx]) * 0.5f);
				}
				else
				{
					wavetables[0][target].wavetable[xCounter + 0] = xbuffer.getWritePointer(0)[xx];
					wavetables[0][target].wavetable[xCounter + 1] =
						xbuffer.getWritePointer(0)[xx] + ((xbuffer.getWritePointer(0)[0] - xbuffer.getWritePointer(0)[xx]) * 0.5f);
				}
				//
				xCounter += 2;
			}
		}
		else
		{
			if ((offset + 600) > xbuffer.getNumSamples()) MessageBox("Waveform is too small: " + String(xbuffer.getNumSamples()), filename);
			int xCounter = 0;
			for (int32 xx = 0; xx < 600; xx++)
			{
				if (applyLimiter)
				{
					wavetables[0][target].wavetable[xCounter + 0] = jlimit(-1.0f, 1.0f, xbuffer.getWritePointer(0)[xx + offset]);
					wavetables[0][target].wavetable[xCounter + 1] = jlimit(-1.0f, 1.0f,
						xbuffer.getWritePointer(0)[xx + offset] + ((xbuffer.getWritePointer(0)[xx + offset + 1] - xbuffer.getWritePointer(0)[xx + offset]) * 0.5f));
				}
				else
				{
					wavetables[0][target].wavetable[xCounter + 0] = xbuffer.getWritePointer(0)[xx + offset];
					wavetables[0][target].wavetable[xCounter + 1] =
						xbuffer.getWritePointer(0)[xx + offset] + ((xbuffer.getWritePointer(0)[xx + offset + 1] - xbuffer.getWritePointer(0)[xx + offset]) * 0.5f);
				}
				//
				xCounter += 2;
			}
		}
		//
		if (fade)
		{
			float fadeCounter = 0.0f;
			float fadeRate = 1.0f / 20.0f;
			//
			for (int32 xx = 0; xx < 20; xx++)
			{
				wavetables[0][target].wavetable[xx] *= fadeCounter;
				fadeCounter += fadeRate;
			}
			//
			fadeCounter = 0.0f;
			for (int32 xx = 1199; xx > (1199 - 20); xx--)
			{
				wavetables[0][target].wavetable[xx] *= fadeCounter;
				fadeCounter += fadeRate;
			}
		}
		//
		wavetables[0][target].wavetable[1200 + 0] = wavetables[0][target].wavetable[0];
		wavetables[0][target].wavetable[1200 + 1] = wavetables[0][target].wavetable[1];
		wavetables[0][target].wavetable[1200 + 2] = wavetables[0][target].wavetable[2];
		//
		waveformNames.add(File(filename).getFileNameWithoutExtension());
	}
	//
	#if WTEST_AUDIO_WRITEFILE
		ScopedPointer<AudioFormatWriter> xWriter;
		StringPairArray metaVals = WavAudioFormat::createBWAVMetadata("", "", "", Time::getCurrentTime(), 0, "");
		//
		File fileWav = "c:\\temp\\" + File(filename).getFileNameWithoutExtension() + ".wav";
		WavAudioFormat xWav;
		ScopedPointer<FileOutputStream> fileWavOutStream = fileWav.createOutputStream();
		xWriter = xWav.createWriterFor(fileWavOutStream, 44100.0f, 1, 16, metaVals, 0);

		ScopedPointer<AudioSampleBuffer> xBuffer = new AudioSampleBuffer(1, 1208);
		xBuffer->clear();
		//
		for (int xs = 0; xs < 1203; xs++)
		{
			xBuffer->setSample(0, xs, wavetables[target].wavetable[xs]);
		}

		xWriter->writeFromAudioSampleBuffer(*xBuffer, 0, xBuffer->getNumSamples());
		xWriter->flush();
		xBuffer = nullptr;

		fileWavOutStream.release();
		//
		xWriter->flush();
		xWriter = nullptr;
	#endif
	//
	reader = nullptr;
}
#endif