#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <type_traits>

#include "environment/Log.h"


static void AlCheckError()
{
	if(alGetError() != AL_NO_ERROR)
	{
		ALenum error = alGetError();
		if(error != AL_NO_ERROR)
		{
			switch(error)
			{
			case AL_INVALID_NAME:
				Ivy::Debug::CoreError("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
				break;
			case AL_INVALID_ENUM:
				Ivy::Debug::CoreError("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
				break;
			case AL_INVALID_VALUE:
				Ivy::Debug::CoreError("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
				break;
			case AL_INVALID_OPERATION:
				Ivy::Debug::CoreError("AL_INVALID_OPERATION: the requested operation is not valid");
				break;
			case AL_OUT_OF_MEMORY:
				Ivy::Debug::CoreError("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
				break;
			default:
				Ivy::Debug::CoreError("UNKNOWN AL ERROR: {}", error);
			}
		}
	}
}

static void AlcCheckError(ALCdevice* device)
{
	ALCenum error = alcGetError(device);
	if(error != ALC_NO_ERROR)
	{
		switch(error)
		{
		case ALC_INVALID_VALUE:
			Ivy::Debug::CoreError("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function");
			break;
		case ALC_INVALID_DEVICE:
			Ivy::Debug::CoreError("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function");
			break;
		case ALC_INVALID_CONTEXT:
			Ivy::Debug::CoreError("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function");
			break;
		case ALC_INVALID_ENUM:
			Ivy::Debug::CoreError("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function");
			break;
		case ALC_OUT_OF_MEMORY:
			Ivy::Debug::CoreError("ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function");
			break;
		default:
			Ivy::Debug::CoreError("UNKNOWN ALC ERROR: {}", error);
		}
	}
}





#define MAX_DATA    16
static const int indentation = 4;
static const int maxmimumWidth = 79;

static void
printChar(int c, int *width)
{
	putchar(c);
	*width = (c == '\n') ? 0 : (*width + 1);
}

static void
indent(int *width)
{
	int i;
	for(i = 0; i < indentation; i++)
	{
		printChar(' ', width);
	}
}

static void
printExtensions(const char *header, char separator, const char *extensions)
{
	int width = 0, start = 0, end = 0;

	printf("%s:\n", header);
	if(extensions == NULL || extensions[0] == '\0')
	{
		return;
	}

	indent(&width);
	while(1)
	{
		if(extensions[end] == separator || extensions[end] == '\0')
		{
			if(width + end - start + 2 > maxmimumWidth)
			{
				printChar('\n', &width);
				indent(&width);
			}
			while(start < end)
			{
				printChar(extensions[start], &width);
				start++;
			}
			if(extensions[end] == '\0')
			{
				break;
			}
			start++;
			end++;
			if(extensions[end] == '\0')
			{
				break;
			}
			printChar(',', &width);
			printChar(' ', &width);
		}
		end++;
	}
	printChar('\n', &width);
}

static char *
getCommandLineOption(int argc, char **argv, const Ivy::String& option)
{
	int slen = option.size();
	char *rv = 0;
	int i;

	for(i = 0; i < argc; i++)
	{
		if(strncmp(argv[i], option.c_str(), slen) == 0)
		{
			i++;
			if(i < argc) rv = argv[i];
		}
	}

	return rv;
}

static char *
getDeviceName(int argc, char **argv)
{
	static char devname[255];
	int len = 255;
	char *s;

	s = getCommandLineOption(argc, argv, "-d");
	if(s)
	{
		strncpy((char *)&devname, s, len);
		len -= strlen(s);

		s = getCommandLineOption(argc, argv, "-r");
		if(s)
		{
			strncat((char *)&devname, " on ", len);
			len -= 4;

			strncat((char *)&devname, s, len);
		}
		s = (char *)&devname;
	}

	return s;
}

static void
displayDevices(const char *type, const char *list)
{
	ALCchar *ptr, *nptr;

	ptr = (ALCchar *)list;
	printf("list of all available %s devices:\n", type);
	if(!list)
	{
		printf("none\n");
	}
	else
	{
		nptr = ptr;
		while(*(nptr += strlen(ptr) + 1) != 0)
		{
			printf("  %s\n", ptr);
			ptr = nptr;
		}
		printf("  %s\n", ptr);
	}
}

static void
testForError(void *p, const Ivy::String& s)
{
	if(p == NULL)
	{
		printf("\nError: %s\n\n", s.c_str());
		exit(-1);
	}
}

static void
testForALCError(ALCdevice *device)
{
	ALenum error;
	error = alcGetError(device);
	if(error != ALC_NO_ERROR)
		printf("\nALC Error %x occurred: %s\n", error, alcGetString(device, error));
}

static void listALInfo()
{
	ALCint data[MAX_DATA];
	ALCdevice *device = NULL;
	ALCcontext *context = NULL;
	ALenum error;
	char *s;

	if(alcIsExtensionPresent(NULL, "ALC_enumeration_EXT") == AL_TRUE)
	{
		if(alcIsExtensionPresent(NULL, "ALC_enumerate_all_EXT") == AL_FALSE)
			s = (char *)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		else
			s = (char *)alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
		displayDevices("output", s);

		s = (char *)alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
		displayDevices("input", s);
	}
	char* c = "";
	s = getDeviceName(0, &c);
	device = alcOpenDevice(s);
	testForError(device, "Audio device not available.");

	context = alcCreateContext(device, NULL);
	testForError(context, "Unable to create a valid context.");

	alcMakeContextCurrent(context);
	testForALCError(device);

	s = (char *)alcGetString(device, ALC_DEFAULT_DEVICE_SPECIFIER);
	printf("default output device: %s\n", s);
	testForALCError(device);

	error = alcIsExtensionPresent(device, "ALC_EXT_capture");
	if(error)
	{
		s = (char *)alcGetString(device, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
		printf("default input device:  %s\n", s);
		testForALCError(device);
	}
	printf("capture support: %s\n", (error) ? "yes" : "no");

	alcGetIntegerv(device, ALC_FREQUENCY, 1, data);
	printf("mixer frequency: %u hz\n", data[0]);
	testForALCError(device);

	alcGetIntegerv(device, ALC_REFRESH, 1, data + 1);
	printf("refresh rate : %u hz\n", data[0] / data[1]);
	testForALCError(device);

	data[0] = 0;
	alcGetIntegerv(device, ALC_MONO_SOURCES, 1, data);
	error = alcGetError(device);
	if(error == AL_NONE)
	{
		printf("supported sources; mono: %u, ", data[0]);

		data[0] = 0;
		alcGetIntegerv(device, ALC_STEREO_SOURCES, 1, data);
		printf("stereo: %u\n", data[0]);
		testForALCError(device);
	}

	printf("ALC version: ");
	alcGetIntegerv(device, ALC_MAJOR_VERSION, 1, data);
	printf("%i.", *data);
	alcGetIntegerv(device, ALC_MINOR_VERSION, 1, data);
	printf("%i\n", *data);
	testForALCError(device);

	s = (char *)alcGetString(device, ALC_EXTENSIONS);
	printExtensions("ALC extensions", ' ', s);
	testForALCError(device);

	s = (char *)alGetString(AL_VENDOR);
	error = alGetError();
	if((error = alGetError()) != AL_NO_ERROR)
		printf("Error #%x: %s\n", error, alGetString(error));
	else
		printf("OpenAL vendor string: %s\n", s);

	s = (char *)alGetString(AL_RENDERER);
	if((error = alGetError()) != AL_NO_ERROR)
		printf("Error #%x: %s\n", error, alGetString(error));
	else
		printf("OpenAL renderer string: %s\n", s);

	s = (char *)alGetString(AL_VERSION);
	if((error = alGetError()) != AL_NO_ERROR)
		printf("Error #%x: %s\n", error, alGetString(error));
	else if(!s)
		printf("Quering AL_VERSION returned NULL pointer!\n");
	else
		printf("OpenAL version string: %s\n", s);

	s = (char *)alGetString(AL_EXTENSIONS);
	printExtensions("OpenAL extensions", ' ', s);
	testForALCError(device);

	/* alut testing mechanism */
	context = alcGetCurrentContext();
	if(context == NULL)
	{
		printf("Error: no current context\n");
	}
	else
	{
		if(alGetError() != AL_NO_ERROR)
		{
			printf("Alert: AL error on entry\n");
		}
		else
		{
			if(alcGetError(alcGetContextsDevice(context)) != ALC_NO_ERROR)
			{
				printf("Alert: ALC error on entry\n");
			}
		}
	}
	/* end of alut test */


	if(alcMakeContextCurrent(NULL) == 0)
		printf("alcMakeContextCurrent failed.\n");

	device = alcGetContextsDevice(context);
	alcDestroyContext(context);
	testForALCError(device);

	if(alcCloseDevice(device) == 0)
		printf("alcCloseDevice failed.\n");
}

namespace Ivy::WavLoader
{
	static std::int32_t convert_to_int(char* buffer, std::size_t len)
	{
		std::int32_t a = 0;
		//if(std::endian::native == std::endian::little)
			std::memcpy(&a, buffer, len);
		//else
		//	for(std::size_t i = 0; i < len; ++i)
		//		reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
		return a;
	}

	static bool load_wav_file_header(std::ifstream& file,
		std::uint8_t& channels,
		std::int32_t& sampleRate,
		std::uint8_t& bitsPerSample,
		ALsizei& size)
	{
		char buffer[4];
		if(!file.is_open())
			return false;

		// the RIFF
		if(!file.read(buffer, 4))
		{
			std::cerr << "ERROR: could not read RIFF" << std::endl;
			return false;
		}
		if(std::strncmp(buffer, "RIFF", 4) != 0)
		{
			std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
			return false;
		}

		// the size of the file
		if(!file.read(buffer, 4))
		{
			std::cerr << "ERROR: could not read size of file" << std::endl;
			return false;
		}

		// the WAVE
		if(!file.read(buffer, 4))
		{
			std::cerr << "ERROR: could not read WAVE" << std::endl;
			return false;
		}
		if(std::strncmp(buffer, "WAVE", 4) != 0)
		{
			std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
			return false;
		}

		// "fmt/0"
		if(!file.read(buffer, 4))
		{
			std::cerr << "ERROR: could not read fmt/0" << std::endl;
			return false;
		}

		// this is always 16, the size of the fmt data chunk
		if(!file.read(buffer, 4))
		{
			std::cerr << "ERROR: could not read the 16" << std::endl;
			return false;
		}

		// PCM should be 1?
		if(!file.read(buffer, 2))
		{
			std::cerr << "ERROR: could not read PCM" << std::endl;
			return false;
		}

		// the number of channels
		if(!file.read(buffer, 2))
		{
			std::cerr << "ERROR: could not read number of channels" << std::endl;
			return false;
		}
		channels = convert_to_int(buffer, 2);

		// sample rate
		if(!file.read(buffer, 4))
		{
			std::cerr << "ERROR: could not read sample rate" << std::endl;
			return false;
		}
		sampleRate = convert_to_int(buffer, 4);

		// (sampleRate * bitsPerSample * channels) / 8
		if(!file.read(buffer, 4))
		{
			std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
			return false;
		}

		// ?? dafaq
		if(!file.read(buffer, 2))
		{
			std::cerr << "ERROR: could not read dafaq" << std::endl;
			return false;
		}

		// bitsPerSample
		if(!file.read(buffer, 2))
		{
			std::cerr << "ERROR: could not read bits per sample" << std::endl;
			return false;
		}
		bitsPerSample = convert_to_int(buffer, 2);

		// data chunk header "data"
		if(!file.read(buffer, 4))
		{
			std::cerr << "ERROR: could not read data chunk header" << std::endl;
			return false;
		}
		if(std::strncmp(buffer, "data", 4) != 0)
		{
			std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
			return false;
		}

		// size of data
		if(!file.read(buffer, 4))
		{
			std::cerr << "ERROR: could not read data size" << std::endl;
			return false;
		}
		size = convert_to_int(buffer, 4);

		/* cannot be at the end of file */
		if(file.eof())
		{
			std::cerr << "ERROR: reached EOF on the file" << std::endl;
			return false;
		}
		if(file.fail())
		{
			std::cerr << "ERROR: fail state set on the file" << std::endl;
			return false;
		}

		return true;
	}

	static char* load_wav(const std::string& filename,
		std::uint8_t& channels,
		std::int32_t& sampleRate,
		std::uint8_t& bitsPerSample,
		ALsizei& size)
	{
		std::ifstream in(filename, std::ios::binary);
		if(!in.is_open())
		{
			std::cerr << "ERROR: Could not open \"" << filename << "\"" << std::endl;
			return nullptr;
		}
		if(!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
		{
			std::cerr << "ERROR: Could not load wav header of \"" << filename << "\"" << std::endl;
			return nullptr;
		}

		char* data = new char[size];

		in.read(data, size);

		return data;
	}

}