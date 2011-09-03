/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2011 Michael Hasselmann <michael@taschenorakel.de>
 *
 *
 * Miniature is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Miniature is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Miniature. If not, see <http://www.gnu.org/licenses/>.
 */

#include "abstractcommand.h"
#include "commands/login.h"
#include "commands/logout.h"
#include "commands/loginfailed.h"
#include "commands/advertisement.h"
#include "commands/record.h"
#include "commands/play.h"
#include "commands/move.h"
#include "commands/creategame.h"
#include "commands/activategame.h"
#include "commands/seek.h"
#include "commands/invalidmove.h"
#include "commands/invalidseek.h"
