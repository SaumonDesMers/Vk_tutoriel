#pragma once

// Define project namespace
#ifndef LIB_NAMESPACE
#	define LIB_NAMESPACE ft
#endif

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
