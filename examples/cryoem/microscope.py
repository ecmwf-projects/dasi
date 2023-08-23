# Copyright 2023 European Centre for Medium-Range Weather Forecasts (ECMWF)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import numpy as np

FAKE_IMG = np.array(
    [
        [0.0, 0.0, 1.0, 1.0, 0.0],
        [0.0, 1.0, 1.0, 0.0, 0.0],
        [1.0, 1.0, 0.0, 0.0, 0.0],
        [1.0, 0.0, 0.0, 1.0, 0.0],
        [0.0, 0.0, 0.0, 1.0, 1.0],
    ],
    dtype=np.float32,
)


class Microscope:
    def __init__(self, num_images=5, metadata=None):
        self.num_images = num_images
        self.metadata = metadata if metadata is not None else {}
        self.metadata["Temperature"] = np.random.uniform(3.0, 4.0)
        self.metadata["ImageSize"] = FAKE_IMG.shape
        self.metadata["NumberOfImages"] = num_images

    def acquire_images(self):
        for i in range(self.num_images):
            img = FAKE_IMG * np.sin(np.pi / 10 * (i + 1))
            img += np.random.normal(scale=0.1, size=img.shape)
            img = np.clip(img, 0.0, 1.0)
            yield img

    def get_metadata(self):
        return self.metadata
