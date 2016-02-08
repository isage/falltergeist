/*
 * Copyright 2012-2016 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

// Related headers
#include "../UI/MultistateImageButton.h"

// C++ standard includes

// Falltergeist includes
#include "../Audio/Mixer.h"
#include "../Event/Event.h"
#include "../Event/Mouse.h"
#include "../Exception.h"
#include "../Game/Game.h"
#include "../Graphics/Texture.h"
#include "../UI/Image.h"

// Third party includes

namespace Falltergeist
{
namespace UI
{

MultistateImageButton::MultistateImageButton(const Point& pos) : Falltergeist::UI::Base(pos)
{
    mouseClickHandler() += std::bind(&MultistateImageButton::_onMouseClick, this, std::placeholders::_1);
}

MultistateImageButton::MultistateImageButton(Type type, int x, int y) : Falltergeist::UI::Base(Point(x, y))
{
    mouseClickHandler() += std::bind(&MultistateImageButton::_onMouseClick, this, std::placeholders::_1);
    mouseUpHandler().add(std::bind(&MultistateImageButton::_onMouseUp, this, std::placeholders::_1));
    switch (type)
    {
        case Type::BIG_SWITCH:
        {
            Image image("art/intrface/prfbknbs.frm");
/* TODO: newrender
            auto image1 = new Image(46, 47);
            image.texture()->copyTo(image1->texture(), 0, 0, 0, 0*47, 46, 47);
            auto image2 = new Image(46, 47);
            image.texture()->copyTo(image2->texture(), 0, 0, 0, 1*47, 46, 47);
            auto image3 = new Image(46, 47);
            image.texture()->copyTo(image3->texture(), 0, 0, 0, 2*47, 46, 47);
            auto image4 = new Image(46, 47);
            image.texture()->copyTo(image4->texture(), 0, 0, 0, 3*47, 46, 47);

            addImage(std::unique_ptr<Image>(image1));
            addImage(std::unique_ptr<Image>(image2));
            addImage(std::unique_ptr<Image>(image3));
            addImage(std::unique_ptr<Image>(image4));
*/
            _downSound = "sound/sfx/ib3p1xx1.acm";
            _upSound = "sound/sfx/ib3lu1x1.acm";
            break;
        }
        case Type::SMALL_SWITCH:
        {
            Image image("art/intrface/prflknbs.frm");
/* TODO: newrender
            auto image1 = new Image(22, 25);
            auto image2 = new Image(22, 50);
            image.texture()->copyTo(image1->texture(), 0, 0, 0, 0, 22, 25);
            image.texture()->copyTo(image2->texture(), 0, 0, 0, 25, 22, 50);
            addImage(std::unique_ptr<Image>(image1));
            addImage(std::unique_ptr<Image>(image2));
*/
            _downSound = "sound/sfx/ib2p1xx1.acm";
            _upSound = "sound/sfx/ib2lu1x1.acm";
            break;
        }
        default:
            throw Exception("MultistateImageButton::MultistateImageButton(unsigned int type, x, y) - unsupported type");
    }
}


MultistateImageButton::MultistateImageButton(ImageList* imageList, const Point& pos) : Falltergeist::UI::Base(pos)
{
    mouseClickHandler() += std::bind(&MultistateImageButton::_onMouseClick, this, std::placeholders::_1);
    for (auto& image : imageList->images())
    {
        // TODO: newrender _imageList.addImage(std::unique_ptr<Image>(new Image(*image)));
    }
}


MultistateImageButton::~MultistateImageButton()
{
}

void MultistateImageButton::addImage(std::unique_ptr<Image> image)
{
    _imageList.addImage(std::move(image));
    _maxState++;
}

unsigned int MultistateImageButton::state() const
{
    return _currentState;
}

void MultistateImageButton::setState(unsigned int state)
{
    _currentState = state;
}

void MultistateImageButton::setMode(Mode mode)
{
    _mode = mode;
}

MultistateImageButton::Mode MultistateImageButton::mode() const
{
    return _mode;
}

void MultistateImageButton::_onMouseClick(Event::Mouse* event)
{
    auto sender = dynamic_cast<MultistateImageButton*>(event->target());

    if (sender->mode() == Mode::PROGRESSION)
    {
        if (sender->modeFactor() > 0)
        {
            sender->_currentState = (sender->_currentState < sender->_maxState - 1) ? sender->_currentState + sender->modeFactor() : 0;
        }
        else
        {
            sender->_currentState = (sender->_currentState > 0) ? sender->_currentState + sender->modeFactor() : sender->_maxState - 1;
        }
    }
    else // Mode::CYCLIC
    {
        if (sender->modeFactor() > 0)
        {
            if (sender->_currentState == sender->_maxState - 1) sender->setModeFactor(-sender->modeFactor());
        }
        else
        {
            if (sender->_currentState == 0) sender->setModeFactor(-sender->modeFactor());
        }
        sender->_currentState += sender->modeFactor();
    }
}

void MultistateImageButton::_onMouseUp(Event::Mouse* event)
{
    if (!event->leftButton()) return;

    auto sender = dynamic_cast<MultistateImageButton*>(event->target());

    if (!sender->_downSound.empty())
    {
        Game::getInstance()->mixer()->playACMSound(sender->_downSound);
    }
    if (!sender->_upSound.empty())
    {
        Game::getInstance()->mixer()->playACMSound(sender->_upSound);
    }
}

Graphics::Texture* MultistateImageButton::texture() const
{
    return _imageList.images().at(_currentState)->texture();
}

void MultistateImageButton::setModeFactor(int factor)
{
    _modeFactor = factor;
}

int MultistateImageButton::modeFactor() const
{
    if (_modeFactor >= 0)
    {
        return 1;
    }
    return -1;
}

void MultistateImageButton::setMaxState(unsigned int value)
{
    _maxState = value;
    if (_currentState > _maxState)
    {
        _currentState = _maxState;
    }
}

unsigned int MultistateImageButton::maxState() const
{
    return _maxState;
}

void MultistateImageButton::setMinState(unsigned int value)
{
    _minState = value;
    if (_currentState < _minState) _currentState = _minState;
}

unsigned int MultistateImageButton::minState() const
{
    return _minState;
}

void MultistateImageButton::handle(Event::Mouse* mouseEvent)
{
    // disable right button clicks
    _rightButtonPressed = false;
    Base::handle(mouseEvent);
}

}
}
