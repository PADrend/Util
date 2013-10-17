/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <Util/Graphics/Bitmap.h>
#include <Util/UI/EventQueue.h>
#include <Util/UI/Event.h>
#include <Util/UI/UI.h>
#include <Util/UI/Window.h>
#include <functional>
#include <iostream>
#include <memory>

class Application {
	private:
		bool running;

		bool showCursor;

		bool inputGrabbed;

		std::unique_ptr<Util::UI::Window> window;

	public:
		Application() : running(true), showCursor(true), inputGrabbed(false), window() {
		}

		bool processEvent(const Util::UI::Event & event) {
			switch(event.type) {
				case Util::UI::EVENT_QUIT:
					running = false;
					std::cout << "QUIT" << std::endl;
					break;
				case Util::UI::EVENT_RESIZE:
					std::cout << "RESIZE: " << event.resize.width << ", " << event.resize.height << std::endl;
					break;
				case Util::UI::EVENT_KEYBOARD:
					std::cout << "KEYBOARD: " << event.keyboard.key << " \'" << event.keyboard.str << "\' " << event.keyboard.pressed;
					if(event.keyboard.pressed) {
						if(event.keyboard.key == Util::UI::KEY_TAB) {
							// 'Tab' toggles cursor
							showCursor = !showCursor;
							if(showCursor) {
								window->setCursor(nullptr);
							} else {
								Util::Reference<Util::Bitmap> bitmap = new Util::Bitmap(1, 1);
								bitmap->data()[0] = 0;
								bitmap->data()[1] = 0;
								bitmap->data()[2] = 0;
								bitmap->data()[3] = 0;
								window->setCursor(Util::UI::createCursor(bitmap, 0, 0));
							}
						} else if(event.keyboard.key == Util::UI::KEY_W) {
							// 'w' warps cursor to (10, 10)
							window->warpCursor(10, 10);
						} else if(event.keyboard.key == Util::UI::KEY_G) {
							// 'g' toggles input grabbing
							if(inputGrabbed) {
								window->ungrabInput();
							} else {
								window->grabInput();
							}
							inputGrabbed = !inputGrabbed;
						} else if(event.keyboard.key == Util::UI::KEY_ESCAPE) {
							// 'Esc' quits the program
							running = false;
						}
					}
					if(event.keyboard.key == Util::UI::KEY_KP0) {
						std::cout << " KeyPad 0";
					} else if(event.keyboard.key == Util::UI::KEY_KP1) {
						std::cout << " KeyPad 1";
					} else if(event.keyboard.key == Util::UI::KEY_KP2) {
						std::cout << " KeyPad 2";
					} else if(event.keyboard.key == Util::UI::KEY_KP3) {
						std::cout << " KeyPad 3";
					} else if(event.keyboard.key == Util::UI::KEY_KP4) {
						std::cout << " KeyPad 4";
					} else if(event.keyboard.key == Util::UI::KEY_KP5) {
						std::cout << " KeyPad 5";
					} else if(event.keyboard.key == Util::UI::KEY_KP6) {
						std::cout << " KeyPad 6";
					} else if(event.keyboard.key == Util::UI::KEY_KP7) {
						std::cout << " KeyPad 7";
					} else if(event.keyboard.key == Util::UI::KEY_KP8) {
						std::cout << " KeyPad 8";
					} else if(event.keyboard.key == Util::UI::KEY_KP9) {
						std::cout << " KeyPad 9";
					}
					std::cout << std::endl;
					break;
				case Util::UI::EVENT_MOUSE_BUTTON:
					std::cout << "BUTTON: ";
					switch(event.button.button) {
						case Util::UI::MOUSE_BUTTON_LEFT:
							std::cout << "left";
							break;
						case Util::UI::MOUSE_BUTTON_MIDDLE:
							std::cout << "middle";
							break;
						case Util::UI::MOUSE_BUTTON_RIGHT:
							std::cout << "right";
							break;
						case Util::UI::MOUSE_WHEEL_UP:
							std::cout << "wheel up";
							break;
						case Util::UI::MOUSE_WHEEL_DOWN:
							std::cout << "wheel down";
							break;
						case Util::UI::MOUSE_BUTTON_OTHER:
						default:
							std::cout << "other";
							break;
					}
					std::cout << ' ' << event.button.pressed <<
						" at (" << event.button.x << ", " << event.button.y << ")" << std::endl;
					break;
				case Util::UI::EVENT_MOUSE_MOTION:
					std::cout << "MOTION: (" << event.motion.x << ", " << event.motion.y << ")";
					if(event.motion.buttonMask & Util::UI::MASK_MOUSE_BUTTON_LEFT) {
						std::cout << " left";
					}
					if(event.motion.buttonMask & Util::UI::MASK_MOUSE_BUTTON_MIDDLE) {
						std::cout << " middle";
					}
					if(event.motion.buttonMask & Util::UI::MASK_MOUSE_BUTTON_RIGHT) {
						std::cout << " right";
					}
					if(event.motion.buttonMask == Util::UI::MASK_NO_BUTTON) {
						std::cout << " no button";
					}
					std::cout << " delta=(" << event.motion.deltaX << ", " << event.motion.deltaY << ")" << std::endl;
					break;
				case Util::UI::EVENT_JOY_AXIS:
					std::cout << "JOYAXIS: " << static_cast<uint16_t>(event.joyAxis.joystick) 
						<< ' ' << static_cast<uint16_t>(event.joyAxis.axis)
						<< ' ' << event.joyAxis.value << std::endl;
					break;
				case Util::UI::EVENT_JOY_BUTTON:
					std::cout << "JOYBUTTON: " << static_cast<uint16_t>(event.joyButton.joystick) 
						<< ' ' << static_cast<uint16_t>(event.joyButton.button) 
						<< ' ' << event.joyButton.pressed << std::endl;
					break;
				case Util::UI::EVENT_JOY_HAT:
					std::cout << "JOYHAT: " << static_cast<uint16_t>(event.joyHat.joystick) 
						<< ' ' << static_cast<uint16_t>(event.joyHat.hat) 
						<< ' ' << static_cast<uint16_t>(event.joyHat.value) << std::endl;
					break;
				default:
					break;
			}
			return true;
		}

		int mainLoop() {
			Util::UI::Window::Properties properties;
			properties.positioned = true;
			properties.posX = 10;
			properties.posY = 10;
			properties.clientAreaWidth = 300;
			properties.clientAreaHeight = 300;
			properties.title = "New Window";
			window.reset(Util::UI::createWindow(properties));
			if(window.get() == nullptr) {
				return 1;
			}
			Util::UI::EventQueue eventQueue;
			eventQueue.registerEventGenerator(std::bind(&Util::UI::Window::fetchEvents, window.get()));
			eventQueue.registerEventHandler(std::bind(&Application::processEvent, this, std::placeholders::_1));
			while(running) {
				eventQueue.process();
				window->swapBuffers();
			}
			window.reset();
			return 0;
		}
};

int main(int /*argc*/, char ** /*argv*/) {
	Application app;
	return app.mainLoop();
}
