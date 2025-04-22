
#include <Windows.h>
#include "MAHMSharedMemory.h"
#include "MAHMSharedMemoryImpl.h"

#include "godot_cpp/variant/utility_functions.hpp"

HANDLE m_hMapFile = NULL;
HANDLE m_pMapAddr = NULL;
LPMAHM_SHARED_MEMORY_HEADER lpHeader = NULL;

// This function is used to disconnect from MSI Afterburner hardware monitoring shared memory
bool MAHM_Disconnect() {
	if (m_pMapAddr) UnmapViewOfFile(m_pMapAddr);
	m_pMapAddr = NULL;

	if (m_hMapFile) CloseHandle(m_hMapFile);
	m_hMapFile = NULL;

	lpHeader = NULL;
	return false;
}

// This function is used to connect to MSI Afterburner hardware monitoring shared memory
bool MAHM_Connect() {
	//W e must disconnect from the previously connected shared memory before connecting to new one
	MAHM_Disconnect();
	
	// Get memory from MSI Afterburner
	m_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MAHMSharedMemory");

	// Get address of the data
	if (m_hMapFile) m_pMapAddr = MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	else return false;

	if(m_pMapAddr) {
		// If we're connected to shared memory, we must check if it is valid or not and reconnect if necessary
		lpHeader = (LPMAHM_SHARED_MEMORY_HEADER)m_pMapAddr;

		// Check shared memory version, allow 2.0 only
		if (lpHeader->dwVersion < 0x00020000) return MAHM_Disconnect();

		return true;
	}
	return false;
}


bool MAHM_UpdateMonitor(godot::Dictionary *output) {
	//get number of data sources
	if(lpHeader == NULL) {
		MAHM_Connect();
		return false;
	}

	if (lpHeader->dwSignature == 0xDEAD) {
		// If MSI is unloaded, notify
		godot::UtilityFunctions::push_warning("MSIAfterburner is unloaded.");
		return MAHM_Disconnect();
	}

	DWORD dwSources = lpHeader->dwNumEntries;
	DWORD dwGpuSources = lpHeader->dwNumGpuEntries;

	//display info for each data source
	for (DWORD dwSource=0; dwSource<dwSources; ++dwSource) {
		// Get pointer to the current data source entry
		LPMAHM_SHARED_MEMORY_ENTRY	lpEntry	= (LPMAHM_SHARED_MEMORY_ENTRY)((LPBYTE)lpHeader + lpHeader->dwHeaderSize + dwSource * lpHeader->dwEntrySize);
		godot::StringName name = lpEntry->szSrcName;
		
		// Store data
		if(output->has(name)) {
			// If entry exists, modify it
			auto &u = output->operator[](name);
			u.set("value", lpEntry->data >= 0xFFFFFFFF ? -1 : lpEntry->data);
			u.set("minLimit", lpEntry->minLimit);
			u.set("maxLimit", lpEntry->maxLimit);
			u.set("unit", lpEntry->szSrcUnits);
		}
		else {
			// If doesn't exist, create it
			godot::Dictionary u;
			u["value"] = lpEntry->data >= 0xFFFFFFFF ? -1 : lpEntry->data;
			u["minLimit"] = lpEntry->minLimit;
			u["maxLimit"] = lpEntry->maxLimit;
			u["unit"] = lpEntry->szSrcUnits;
			
			output->operator[](name) = u;
		}
	}

	// Store gpu info
	for (DWORD dwGpu=0; dwGpu < dwGpuSources; ++dwGpu) {
		LPMAHM_SHARED_MEMORY_GPU_ENTRY lpGpuEntry = (LPMAHM_SHARED_MEMORY_GPU_ENTRY)((LPBYTE)lpHeader + lpHeader->dwHeaderSize + lpHeader->dwNumEntries * lpHeader->dwEntrySize + dwGpu * lpHeader->dwGpuEntrySize);
		
		godot::StringName name = godot::String("GPU") + godot::String((godot::Variant) ((int) dwGpu + 1)); // GPU1, GPU2, ...

		output->operator[](name) = lpGpuEntry -> szDevice;
	}
	return true;
}