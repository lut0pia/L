#pragma once

#include <L/src/container/Buffer.h>
#include <L/src/container/Table.h>
#include <L/src/engine/Resource.h>
#include <L/src/time/Date.h>

#include "rmlui.h"

class RmlUiFile : public Rml::Core::FileInterface {
protected:
	struct File {
		L::Resource<L::Buffer> resource;
		uintptr_t offset = 0;
	};
	L::Table<Rml::Core::FileHandle, File> _files;
	L::Array<L::Resource<L::Buffer>> _resources;
	Rml::Core::FileHandle _next_handle = 1;
	L::Date _mtime = 0;

public:
	virtual Rml::Core::FileHandle Open(const Rml::Core::String& path) override;
	virtual void Close(Rml::Core::FileHandle file) override;

	virtual size_t Read(void* buffer, size_t size, Rml::Core::FileHandle file) override;
	virtual bool Seek(Rml::Core::FileHandle file, long offset, int origin) override;
	virtual size_t Tell(Rml::Core::FileHandle file) override;

	virtual size_t Length(Rml::Core::FileHandle file) override;

	bool HasOutOfDateResources();
};
