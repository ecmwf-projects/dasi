
import numpy as np

FAKE_IMG = np.array([[0., 0., 1., 1., 0.],
                     [0., 1., 1., 0., 0.],
                     [1., 1., 0., 0., 0.],
                     [1., 0., 0., 1., 0.],
                     [0., 0., 0., 1., 1.]],
                    dtype=np.float32)

class Microscope:
    def __init__(self, num_images=5, metadata=None):
        self.num_images = num_images
        self.metadata = metadata if metadata is not None else {}
        self.metadata["Temperature"] = np.random.uniform(3., 4.)
        self.metadata["ImageSize"] = FAKE_IMG.shape
        self.metadata["NumberOfImages"] = num_images

    def acquire_images(self):
        for i in range(self.num_images):
            img = FAKE_IMG * np.sin(np.pi / 10 * (i + 1))
            img += np.random.normal(scale=0.1, size=img.shape)
            img = np.clip(img, 0., 1.)
            yield img

    def get_metadata(self):
        return self.metadata