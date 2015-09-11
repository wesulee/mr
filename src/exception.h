#pragma once

#include <cstddef>
#include <string>


namespace SDLFunc {
	constexpr char SDL_CreateRenderer[] = "SDL_CreateRenderer";
	constexpr char SDL_CreateRGBSurface[] = "SDL_CreateRGBSurface";
	constexpr char SDL_CreateTextureFromSurface[] = "SDL_CreateTextureFromSurface";
	constexpr char SDL_CreateWindow[] = "SDL_CreateWindow";
	constexpr char SDL_GetNumRenderDrivers[] = "SDL_GetNumRenderDrivers";
	constexpr char SDL_GetRenderDriverInfo[] = "SDL_GetRenderDriverInfo";
	constexpr char SDL_GetRendererInfo[] = "SDL_GetRendererInfo";
	constexpr char SDL_LoadBMP[] = "SDL_LoadBMP";
	constexpr char SDL_QueryTexture[] = "SDL_QueryTexture";
	constexpr char SDL_RegisterEvents[] = "SDL_RegisterEvents";
	constexpr char SDL_RenderReadPixels[] = "SDL_RenderReadPixels";
	constexpr char SDL_RenderSetClipRect[] = "SDL_RenderSetClipRect";
	constexpr char SDL_RenderSetViewport[] = "SDL_RenderSetViewport";
	constexpr char SDL_SetRenderDrawBlendMode[] = "SDL_SetRenderDrawBlendMode";
	constexpr char SDL_SetTextureAlphaMod[] = "SDL_SetTextureAlphaMod";
	constexpr char TTF_GlyphMetrics[] = "TTF_GlyphMetrics";
	constexpr char TTF_OpenFont[] = "TTF_OpenFont";
	constexpr char TTF_RenderText_[] = "TTF_RenderText_*";
	constexpr char TTF_SizeText[] = "TTF_SizeText";
}


class Exception {
public:
	Exception() = default;
	virtual ~Exception() {}
	virtual std::string what(void) const = 0;
};


class RuntimeError : public Exception {
public:
	RuntimeError(const std::string&, const std::string& = std::string{});
	~RuntimeError() = default;
	std::string what(void) const override;
private:
	std::string message;
	std::string details;
};


// NOTE: should only be constructed immediately after SDL error
class SDLError : public Exception {
public:
	SDLError(const std::string&, const char*);
	~SDLError() = default;
	std::string what(void) const override;
private:
	std::string message;
	std::string sdlMessage;
	const char* funcName;
};


class BadData : public Exception {
public:
	BadData(const std::string&, const std::string& = std::string{});
	~BadData() = default;
	std::string what(void) const override;
	void setFilePath(const std::string&);
	void setDetails(const std::string&);
private:
	std::string filePath;
	std::string message;
	std::string details;
};


class FileError : public Exception {
public:
	enum class Err {MISSING, NOT_OPEN, NOT_FILE, NOT_DIRECTORY};
	FileError(const std::string&, const std::string&, const std::string& = std::string{});
	FileError(const std::string&, const Err, const std::string& = std::string{});
	~FileError() = default;
	std::string what(void) const override;
private:
	std::string filePath;
	std::string message;
	std::string details;
};


class ParserError : public Exception {
public:
	enum class DataType {NONE, INI, JSON};

	ParserError(const DataType = DataType::NONE);
	~ParserError() = default;
	std::string what(void) const override;
	void setPath(const std::string&);
	void setWhat(const std::string&, const std::string& = std::string{});
	void setType(const DataType);
	void setLine(const std::size_t);
	void setOffset(const std::size_t);
	void incLine(void);
private:
	std::string filePath;
	std::string message;
	std::string details;
	std::size_t line;
	std::size_t offset;
	DataType type;
};


inline
void BadData::setFilePath(const std::string& path) {
	filePath = path;
}


inline
void BadData::setDetails(const std::string& det) {
	details = det;
}


inline
void ParserError::incLine() {
	++line;
}
