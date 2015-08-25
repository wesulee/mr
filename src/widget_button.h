#pragma once

#include "color.h"
#include "sdl_helper.h"
#include "widget.h"
#include <functional>
#include <memory>
#include <string>


class TextRenderer;


typedef std::function<void(void)> ButtonCallback;


class AbstractButton : public Widget {
public:
	enum class CallbackType {ON_PRESS, ON_RELEASE, ON_CLICK};
	AbstractButton() = default;
	virtual ~AbstractButton() = default;
	virtual void setCallback(ButtonCallback, const CallbackType=CallbackType::ON_RELEASE);
	// some Widget implementation
	void event(WidgetEvent&) override;
protected:
	ButtonCallback callback;
	WidgetState state = WidgetState::OUT;
	CallbackType cType = CallbackType::ON_RELEASE;
	bool pressed = false;
};


class TextButtonBase : public AbstractButton {
public:
	TextButtonBase() = default;
	virtual ~TextButtonBase() = default;
	virtual void setText(const std::string&);
	const std::string& getText(void) const;
	const SDL_Rect& getTextBounds(void) const;
	// Widget implementation
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
protected:
	virtual bool test(void) const = 0;	// is texture set?
	virtual IntPair prefSizeOf(const IntPair&) const = 0;
	static IntPair calcPrefSize(const IntPair&, const int, const int);

	std::string text;
	SDL_Rect texBounds;
};


// cuts off text rather than resize when size too small
// texBounds relative to inside border
class TextButton : public TextButtonBase {
	TextButton(const TextButton&) = delete;
	void operator=(const TextButton&) = delete;
public:
	struct Style {
		TextRenderer* tr = nullptr;
		int outlineSz = 0;
		int padTextH = 5;	// min horiz padding around text
		int padTextV = 5;
		Color colText;
		Color colOutline;
		Color colBgOut;
		Color colBgOver;
		Color colBgDown;
	};
	TextButton();
	~TextButton();
	void setStyle(const std::shared_ptr<Style>&);
	void setText(const std::string&) override;	// this will resize button
	// Widget implementation
	void draw(Canvas&) override;
protected:
	bool test(void) const override;
	IntPair prefSizeOf(const IntPair&) const override;
private:
	static const Color& select(const Style&, const WidgetState);
	std::shared_ptr<Style> style;
	SDL_Texture* tex = nullptr;
};


// like TextButton, but state-varying text color and button alpha
class TextButton2 : public TextButtonBase {
	TextButton2(const TextButton2&) = delete;
	void operator=(const TextButton2&) = delete;
public:
	struct Style {
		TextRenderer* tr = nullptr;
		int outlineSz = 0;
		int padTextH = 5;	// min horiz padding around text
		int padTextV = 5;
		Color colOutline;
		Color colTextOut;
		Color colTextOver;
		Color colTextDown;
		Color colBgOut;
		Color colBgOver;
		Color colBgDown;
		Uint8 alphaOut = SDL_ALPHA_OPAQUE;	// alpha of entire button
		Uint8 alphaOver = SDL_ALPHA_OPAQUE;
		Uint8 alphaDown = SDL_ALPHA_OPAQUE;
	};
	TextButton2();
	~TextButton2();
	void setStyle(const std::shared_ptr<Style>&);
	void setText(const std::string&) override;	// this will resize button
	// Widget implementation
	void draw(Canvas&) override;
protected:
	bool test(void) const override;
	IntPair prefSizeOf(const IntPair&) const override;
private:
	std::shared_ptr<Style> style;
	SDL_Texture* texOut = nullptr;
	SDL_Texture* texOver = nullptr;
	SDL_Texture* texDown = nullptr;
};


// Note: all textures must be same size
class BitmapButton : public AbstractButton {
	BitmapButton(const BitmapButton&) = delete;
	void operator=(const BitmapButton&) = delete;
public:
	BitmapButton();
	~BitmapButton();
	// takes ownership of texture
	void setTexture(const WidgetState, SDL_Texture*);
	// Widget implementation
	void draw(Canvas&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	SDL_Texture*& getTex(const WidgetState);

	IntPair texDim;	// texture dimensions
	SDL_Texture* texOut = nullptr;
	SDL_Texture* texOver = nullptr;
	SDL_Texture* texDown = nullptr;
};


inline
void AbstractButton::setCallback(ButtonCallback func, const CallbackType ct) {
	callback = func;
	cType = ct;
}


inline
void TextButtonBase::setText(const std::string& str) {
	text = str;
}


inline
const std::string& TextButtonBase::getText() const {
	return text;
}


inline
const SDL_Rect& TextButtonBase::getTextBounds() const {
	return texBounds;
}
