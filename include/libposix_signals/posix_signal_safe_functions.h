#pragma once

// TODO
// When inside a signal handler, a restricted amount of functions is allowed to be used
// in order to stay safe with context changes.

// To ensure that this rule is correctly followed, callbacks called from signal handling
// will be only able to use these functions.
