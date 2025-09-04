//! Demonstrates the most barebone usage of the Rerun SDK.

use std::{collections::HashMap, env::VarError, path::Path};

use dora_node_api::{
    arrow::array::{Array, Float32Array, Float64Array, StringArray, UInt8Array},
    DoraNode, Event, Parameter,
};
use eyre::{eyre, Context, ContextCompat, Result};

use rerun::{
    components::ImageBuffer,
    external::{log::warn, re_types::ArrowBuffer},
    ImageFormat, Points3D, SpawnOptions,
};
pub mod boxes2d;

pub fn lib_main() -> Result<()> {
    let (node, mut events) = DoraNode::init_from_env().context("Could not initialize dora node")?;

    // Setup an image cache to paint depth images.
    let mut options = SpawnOptions::default();

    let memory_limit = match std::env::var("RERUN_MEMORY_LIMIT") {
        Ok(memory_limit) => memory_limit
            .parse::<String>()
            .context("Could not parse RERUN_MEMORY_LIMIT value")?,
        Err(VarError::NotUnicode(_)) => {
            return Err(eyre!("RERUN_MEMORY_LIMIT env variable is not unicode"));
        }
        Err(VarError::NotPresent) => "25%".to_string(),
    };

    options.memory_limit = memory_limit;

    let rec = match std::env::var("OPERATING_MODE").as_deref() {
        Ok("SPAWN") => rerun::RecordingStreamBuilder::new("dora-rerun")
            .spawn_opts(&options, None)
            .context("Could not spawn rerun visualization")?,
        Ok("SAVE") => {
            let id = node.dataflow_id();
            let path = Path::new("out")
                .join(id.to_string())
                .join(format!("archive-{}.rerun", id));

            rerun::RecordingStreamBuilder::new("dora-rerun")
                .save(path)
                .context("Could not save rerun visualization")?
        }
        Ok(_) => {
            warn!("Invalid operating mode, defaulting to SPAWN mode.");

            rerun::RecordingStreamBuilder::new("dora-rerun")
                .spawn_opts(&options, None)
                .context("Could not spawn rerun visualization")?
        }
        Err(_) => rerun::RecordingStreamBuilder::new("dora-rerun")
            .spawn_opts(&options, None)
            .context("Could not spawn rerun visualization")?,
    };

    match std::env::var("README") {
        Ok(readme) => {
            readme
                .parse::<String>()
                .context("Could not parse readme value")?;
            rec.log("README", &rerun::TextDocument::new(readme))
                .wrap_err("Could not log text")?;
        }
        Err(VarError::NotUnicode(_)) => {
            return Err(eyre!("readme env variable is not unicode"));
        }
        Err(VarError::NotPresent) => (),
    };

    while let Some(event) = events.recv() {
        if let Event::Input { id, data, metadata } = event {
            if id.as_str().contains("image") {
                let height = &480;
                let width = &640;
                let encoding = "rgb8";

                if encoding == "rgb8" {
                    let buffer: &UInt8Array = data.as_any().downcast_ref().unwrap();
                    let byte_count = buffer.len();
                    println!("The buffer occupies {} bytes.", byte_count);

                    let buffer: &[u8] = buffer.values();
                    let buffer = ArrowBuffer::from(buffer);
                    let image_buffer = ImageBuffer::try_from(buffer)
                        .context("Could not convert buffer to image buffer")?;

                    let image = rerun::Image::new(
                        image_buffer,
                        ImageFormat::rgb8([*width as u32, *height as u32]),
                    );
                    rec.log(id.as_str(), &image)
                        .context("could not log image")?;
                }
            } else if id.as_str().contains("text") {
                let buffer: StringArray = data.to_data().into();
                buffer.iter().try_for_each(|string| -> Result<()> {
                    if let Some(str) = string {
                        rec.log(id.as_str(), &rerun::TextLog::new(str))
                            .wrap_err("Could not log text")
                    } else {
                        Ok(())
                    }
                })?;
            } else if id.as_str().contains("boxes2d") {
                boxes2d::update_boxes2d(&rec, id, data, metadata).context("update boxes 2d")?;
            } else {
                println!("Could not find handler for {}", id);
            }
        }
    }

    Ok(())
}

#[cfg(feature = "python")]
use pyo3::{
    pyfunction, pymodule,
    types::{PyModule, PyModuleMethods},
    wrap_pyfunction, Bound, PyResult, Python,
};

#[cfg(feature = "python")]
#[pyfunction]
fn py_main(_py: Python) -> eyre::Result<()> {
    lib_main()
}

/// A Python module implemented in Rust.
#[cfg(feature = "python")]
#[pymodule]
fn dora_rerun(_py: Python, m: Bound<'_, PyModule>) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(py_main, &m)?)?;
    m.add("__version__", env!("CARGO_PKG_VERSION"))?;
    Ok(())
}
