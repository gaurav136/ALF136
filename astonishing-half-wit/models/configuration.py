from dataclasses import dataclass, asdict, field, fields
from math import inf
import json
import time


@dataclass
class Config:
    threshold_max: int = field(default=255)
    threshold: int = field(default=30)
    threshold_min: int = field(default=0)
    blur_size: int = field(default=3)
    area_max: int = field(default=inf)
    area_min: int = field(default=-1)
    block_size: int = field(default=3)
    c: float = field(default=3)
    adaptive_threshold: bool = field(default=False)
    quad_precision: float = field(default=0.001)

    @staticmethod
    def from_dict(argDict):
        fieldSet = {f.name for f in fields(Config) if f.init}
        filteredArgDict = {k: v for k, v in argDict.items() if k in fieldSet}
        return Config(**filteredArgDict)

    @staticmethod
    def load_config(path: str = "config.json"):
        try:
            with open(path, "r") as fp:
                config_dict: dict = json.loads(fp.read())
                config_dict.pop("saved_time", None)
                return Config.from_dict(config_dict)
        except (FileNotFoundError, IsADirectoryError, PermissionError):
            return Config()

    def save_config(self, path: str = "config.json"):
        with open(path, "w") as fp:
            config_dict = self.to_dict()
            config_dict["saved_time"] = time.time()
            fp.write(json.dumps(config_dict))

    def to_dict(self):
        return asdict(self)


if __name__ == "__main__":

    # For testing
    from pprint import pprint
    config = Config()
    # config.save_config()
    config = Config.load_config()
    pprint(config)
