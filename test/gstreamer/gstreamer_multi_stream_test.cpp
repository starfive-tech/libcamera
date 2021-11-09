/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2021, Vedant Paranjape
 *
 * gstreamer_multi_stream_test.cpp - GStreamer multi stream capture test
 */

#include <iostream>
#include <unistd.h>

#include <libcamera/libcamera.h>

#include <gst/gst.h>

#include "gstreamer_test.h"
#include "test.h"

using namespace std;

class GstreamerMultiStreamTest : public GstreamerTest, public Test
{
public:
	GstreamerMultiStreamTest()
		: GstreamerTest()
	{
	}

protected:
	int init() override
	{
		if (status_ != TestPass)
			return status_;

		/* Check if platform supports multistream capture */
		libcamera::CameraManager cm;
		cm.start();
		bool cameraFound = false;
		for (auto &camera : cm.cameras()) {
			if (camera->streams().size() > 1) {
				cameraName_ = camera->id();
				cameraFound = true;
				cm.stop();
				break;
			}
		}

		if (!cameraFound) {
			cm.stop();
			return TestSkip;
		}

		const gchar *streamDescription = "queue ! fakesink";
		g_autoptr(GError) error = NULL;

		stream0_ = gst_parse_bin_from_description_full(streamDescription, TRUE,
							       NULL,
							       GST_PARSE_FLAG_FATAL_ERRORS,
							       &error);
		if (!stream0_) {
			g_printerr("Stream0 could not be created (%s)\n", error->message);
			return TestFail;
		}
		g_object_ref_sink(stream0_);

		stream1_ = gst_parse_bin_from_description_full(streamDescription, TRUE,
							       NULL,
							       GST_PARSE_FLAG_FATAL_ERRORS,
							       &error);
		if (!stream1_) {
			g_printerr("Stream1 could not be created (%s)\n", error->message);
			return TestFail;
		}
		g_object_ref_sink(stream1_);

		if (createPipeline() != TestPass)
			return TestFail;

		return TestPass;
	}

	int run() override
	{
		g_object_set(libcameraSrc_, "camera-name", cameraName_.c_str(), NULL);

		/* Build the pipeline */
		gst_bin_add_many(GST_BIN(pipeline_), libcameraSrc_,
				 stream0_, stream1_, NULL);

		g_autoptr(GstPad) src_pad = gst_element_get_static_pad(libcameraSrc_, "src");
		g_autoptr(GstPad) request_pad = gst_element_get_request_pad(libcameraSrc_, "src_%u");

		{
			g_autoptr(GstPad) queue0_sink_pad = gst_element_get_static_pad(stream0_, "sink");
			g_autoptr(GstPad) queue1_sink_pad = gst_element_get_static_pad(stream1_, "sink");

			if (gst_pad_link(src_pad, queue0_sink_pad) != GST_PAD_LINK_OK ||
			    gst_pad_link(request_pad, queue1_sink_pad) != GST_PAD_LINK_OK) {
				g_printerr("Pads could not be linked.\n");
				return TestFail;
			}
		}

		if (startPipeline() != TestPass)
			return TestFail;

		if (processEvent() != TestPass)
			return TestFail;

		return TestPass;
	}

	void cleanup() override
	{
		g_clear_object(&stream0_);
		g_clear_object(&stream1_);
	}

private:
	std::string cameraName_;
	GstElement *stream0_;
	GstElement *stream1_;
};

TEST_REGISTER(GstreamerMultiStreamTest)
