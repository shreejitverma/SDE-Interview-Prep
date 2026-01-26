/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

import FancyLogger from './fancyLogger.js'

const logger = new FancyLogger()

export default function logSecondImplementation() {
  logger.printLogCount()
  logger.log('Second file')
  logger.printLogCount()
}