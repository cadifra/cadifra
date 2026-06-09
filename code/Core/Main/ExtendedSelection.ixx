/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:ExtendedSelection;

namespace Core
{

export {
class ElementSet;
class IElement;
class IDiagram;
}


export auto extendedSelection(const ElementSet& selection, IDiagram&)
    -> ElementSet;

}
