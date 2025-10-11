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

void Add(Core::Env&, Core::IContainable&) override;
void Forget(Core::Env&, const Core::IContainable&) override;
bool Has(const Core::IContainable&) const override;
void InitialAdd(Core::Env&, Core::IContainable&) override;
