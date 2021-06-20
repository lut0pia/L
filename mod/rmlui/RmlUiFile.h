#pragma once

#include <L/src/container/Buffer.h>
#include <L/src/container/Table.h>
#include <L/src/engine/Resource.h>
#include <L/src/time/Date.h>

#include "rmlui.h"

class RmlUiFile : public Rml::FileInterface {
protected:
	struct File {
		L::Resource<L::Buffer> resource;
		uintptr_t offset = 0;
	};
	L::Table<Rml::FileHandle, File> _files;
	L::Array<L::Resource<L::Buffer>> _resources;
	Rml::FileHandle _next_handle = 1;
	L::Date _mtime = 0;

public:
	virtual Rml::FileHandle Open(const Rml::String& path) override;
	virtual void Close(Rml::FileHandle file) override;

	virtual size_t Read(void* buffer, size_t size, Rml::FileHandle file) override;
	virtual bool Seek(Rml::FileHandle file, long offset, int origin) override;
	virtual size_t Tell(Rml::FileHandle file) override;

	virtual size_t Length(Rml::FileHandle file) override;

	bool HasOutOfDateResources();
};
