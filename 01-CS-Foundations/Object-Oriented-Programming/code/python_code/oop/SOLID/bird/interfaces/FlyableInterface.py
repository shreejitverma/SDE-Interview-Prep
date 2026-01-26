# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

from abc import ABC, abstractmethod

class FlyableInterface(ABC):
    @abstractmethod
    def fly() -> None:
        ...