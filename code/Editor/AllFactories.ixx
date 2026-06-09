/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.AllFactories;

import Store.IStorable;

namespace Editor
{

export namespace AllFactories
{
void report(Store::IFactoryReporter& r);
// calls r.report(x) for every Store::IFactory "x".
}

}
