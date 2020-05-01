/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2020, Raspberry Pi (Trading) Ltd.
 *
 * dng_writer.h - DNG writer
 */
#ifndef __LIBCAMERA_DNG_WRITER_H__
#define __LIBCAMERA_DNG_WRITER_H__

#ifdef HAVE_TIFF
#define HAVE_DNG

#include <libcamera/buffer.h>
#include <libcamera/camera.h>
#include <libcamera/stream.h>

using namespace libcamera;

class DNGWriter
{
public:
	static int write(const char *filename, const Camera *camera,
			 const StreamConfiguration &config,
			 const FrameBuffer *buffer, const void *data);
};

#endif /* HAVE_TIFF */

#endif /* __LIBCAMERA_DNG_WRITER_H__ */
