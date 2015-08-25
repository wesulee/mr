#pragma once

#include <memory>


class DialogData;
class TextRenderer;


class WidgetData {
public:
	void init(TextRenderer* const);

	std::shared_ptr<DialogData> dialogData;
	TextRenderer* defaultTR = nullptr;
	int buttonDefaultW = 0;
	int buttonDefaultH = 0;
};
