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


void resize(Core::Env&, const d1::nRect&, const ResizeMode*) override;
void setPosition(Core::Env&, const d1::Point&) override;
d1::Point pos() const override;
d1::Point oldPos() const override;
d1::Size size() const override;
d1::Size oldSize() const override;
