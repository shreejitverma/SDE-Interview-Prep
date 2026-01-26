# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

from abc import ABC, abstractmethod

class FlyingBehaviourInterface(ABC):
    @abstractmethod
    def makeFly() -> None:
        ...