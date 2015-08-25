#pragma once

#include <string>


class Exception;


namespace SDLFunc {
	constexpr char SDL_CreateRGBSurface[] = "SDL_CreateRGBSurface";
	constexpr char SDL_CreateTextureFromSurface[] = "SDL_CreateTextureFromSurface";
	constexpr char SDL_LoadBMP[] = "SDL_LoadBMP";
	constexpr char SDL_QueryTexture[] = "SDL_QueryTexture";
	constexpr char SDL_RegisterEvents[] = "SDL_RegisterEvents";
	constexpr char SDL_RenderReadPixels[] = "SDL_RenderReadPixels";
	constexpr char SDL_RenderSetClipRect[] = "SDL_RenderSetClipRect";
	constexpr char SDL_RenderSetViewport[] = "SDL_RenderSetViewport";
	constexpr char SDL_SetTextureAlphaMod[] = "SDL_SetTextureAlphaMod";
	constexpr char TTF_GlyphMetrics[] = "TTF_GlyphMetrics";
	constexpr char TTF_RenderText_[] = "TTF_RenderText_*";
	constexpr char TTF_SizeText[] = "TTF_SizeText";
}


void logAndExit(const Exception&);


class Exception {
public:
	Exception() = default;
	virtual ~Exception() {}
	virtual std::string what(void) const = 0;
};


class RuntimeError : public Exception {
public:
	RuntimeError(const std::string&, const std::string& = std::string{});
	std::string what(void) const override;
private:
	std::string message;
	std::string details;
};


// NOTE: should only be constructed immediately after SDL error
class SDLError : public Exception {
public:
	SDLError(const std::string&, const char*);
	std::string what(void) const override;
private:
	std::string message;
	std::string sdlMessage;
	const char* funcName;
};


class BadData : public Exception {
public:
	BadData(const std::string&, const std::string& = std::string{});
	std::string what(void) const override;
private:
	std::string message;
	std::string details;
};


class FileError : public Exception {
public:
	enum class Err {MISSING, NOT_OPEN, NOT_FILE, NOT_DIRECTORY};
	FileError(const std::string&, const std::string&, const std::string& = std::string{});
	FileError(const std::string&, const Err, const std::string& = std::string{});
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
	std::string what(void) const override;
	void setPath(const std::string&);
	void setWhat(const std::string&, const std::string& = std::string{});
	void setType(const DataType);
	void setLine(const int);
	void incLine(void);
private:
	std::string filePath;
	std::string message;
	std::string details;
	int line = 0;
	DataType type;
};


inline
void ParserError::incLine() {
	++line;
}
