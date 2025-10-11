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


void Resize(Core::Env&, const d1::nRect&, const ResizeMode*) override;
void SetPosition(Core::Env&, const d1::Point&) override;
d1::Point Pos() const override;
d1::Point OldPos() const override;
d1::Size Size() const override;
d1::Size OldSize() const override;
