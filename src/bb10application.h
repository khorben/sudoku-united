/**
    This file is part of Sudoku United.

    Sudoku United is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Sudoku United is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sudoku United.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BB10APPLICATION_H
#define BB10APPLICATION_H

#include <bb/cascades/Application>

class BB10Application : public bb::cascades::Application
{
    Q_OBJECT

public:
    static BB10Application *create(int &argc, char *argv[]);

private slots:
    void pauseGame();
    void unpauseGame();

private:
    BB10Application(int &argc, char *argv[]);

    void registerNativeComponents();
    void registerSystemUIComponents();

    void createCover();
    void createRootComponent();
};

#endif // BB10APPLICATION_H
