/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

//
//                              * * *
//
//       WARNING: THIS FILE IS NOT AN ORDINARY HEADER FILE.
//
//                              * * *
//

void add(Core::Env&, Core::IContainable&) override;
void forget(Core::Env&, const Core::IContainable&) override;
bool has(const Core::IContainable&) const override;
void initialAdd(Core::Env&, Core::IContainable&) override;
