/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Copyright 2013 Medium Entertainment, Inc.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 PHPublisherOpenRequest.h
 playhaven-sdk-ios

 Created by Jesus Fernandez on 3/30/11.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#import <Foundation/Foundation.h>
#import "PHAPIRequest.h"

/**
 * @internal
 *
 * @brief Open request used to report the beginning of game sessions.
 **/
@interface PHPublisherOpenRequest : PHAPIRequest {
    NSString *_customUDID;
}

@property (nonatomic, copy) NSString *customUDID; /**< Publishers can attach an arbitrary user identifier to the open request by setting
                                                       this property. This will be appended to the request as a d_custom parameter. */
@end
