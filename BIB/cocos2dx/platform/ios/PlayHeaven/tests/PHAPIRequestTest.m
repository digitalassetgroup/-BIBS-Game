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

 PHAPIRequestTest.m
 playhaven-sdk-ios

 Created by Jesus Fernandez on 3/30/11.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#import <SenTestingKit/SenTestingKit.h>
#import "PHAPIRequest.h"
#import "PHConstants.h"
#import "PHStringUtil.h"

#define PUBLISHER_TOKEN @"PUBLISHER_TOKEN"
#define PUBLISHER_SECRET @"PUBLISHER_SECRET"

#define HASH_STRING  @"DEVICE_ID:PUBLISHER_TOKEN:PUBLISHER_SECRET:NONCE"
#define EXPECTED_HASH @"3L0xlrDOt02UrTDwMSnye05Awwk"

@interface PHAPIRequest (Private)
+ (NSMutableSet *)allRequests;
+ (void)setSession:(NSString *)session;
- (void)processRequestResponse:(NSDictionary *)response;
@end

@interface PHAPIRequestTest : SenTestCase @end
@interface PHAPIRequestResponseTest : SenTestCase <PHAPIRequestDelegate> {
    PHAPIRequest *_request;
    BOOL _didProcess;
}
@end
@interface PHAPIRequestErrorTest : SenTestCase <PHAPIRequestDelegate> {
    PHAPIRequest *_request;
    BOOL _didProcess;
}
@end
@interface PHAPIRequestByHashCodeTest : SenTestCase @end

@implementation PHAPIRequestTest

- (void)testSignatureHash
{
    NSString *signatureHash = [PHAPIRequest base64SignatureWithString:HASH_STRING];
    STAssertTrue([EXPECTED_HASH isEqualToString:signatureHash],
                 @"Hash mismatch. Expected %@ got %@",EXPECTED_HASH,signatureHash);
}

- (void)testResponseDigestVerification
{
    /*
     For this test expected digest hashes were generated using pyton's hmac library.
     */
    NSString *responseDigest, *expectedDigest;

    // Digest with nonce
    responseDigest = [PHAPIRequest expectedSignatureValueForResponse:@"response body" nonce:@"nonce" secret:PUBLISHER_SECRET];
    expectedDigest = @"rt3JHGReRAaol-xPVildr6Ev9fU=";
    STAssertTrue([responseDigest isEqualToString:expectedDigest], @"Digest mismatch. Expected %@ got %@", expectedDigest, responseDigest);

    // Digest without nonce
    responseDigest = [PHAPIRequest expectedSignatureValueForResponse:@"response body" nonce:nil secret:PUBLISHER_SECRET];
    expectedDigest = @"iNmo12xRqVAn_7quEvOSwhenEZA=";
    STAssertTrue([responseDigest isEqualToString:expectedDigest], @"Digest mismatch. Expected %@ got %@", expectedDigest, responseDigest);
}

- (void)testRequestParameters
{
    PHAPIRequest *request = [PHAPIRequest requestForApp:PUBLISHER_TOKEN secret:PUBLISHER_SECRET];
    NSDictionary *signedParameters = [request signedParameters];

    // Test for existence of parameters
    NSString
        *session   = [signedParameters valueForKey:@"session"],
        *gid       = [signedParameters valueForKey:@"gid"],
        *token     = [signedParameters valueForKey:@"token"],
        *signature = [signedParameters valueForKey:@"signature"],
        *nonce     = [signedParameters valueForKey:@"nonce"];

    STAssertNotNil(session ,@"Required session param is missing!");
    STAssertNotNil(gid ,@"Required gid param is missing!");
    STAssertNotNil(token ,@"Required token param is missing!");
    STAssertNotNil(signature,@"Required signature param is missing!");
    STAssertNotNil(nonce ,@"Required nonce param is missing!");

    NSString *parameterString = [request signedParameterString];
    STAssertNotNil(parameterString, @"Parameter string is nil?");

    NSString *tokenParam = [NSString stringWithFormat:@"token=%@",token];
    STAssertFalse([parameterString rangeOfString:tokenParam].location == NSNotFound,
                  @"Token parameter not present!");

    NSString *signatureParam = [NSString stringWithFormat:@"signature=%@",signature];
    STAssertFalse([parameterString rangeOfString:signatureParam].location == NSNotFound,
                  @"Signature parameter not present!");

    NSString
        *expectedSignatureString = [NSString stringWithFormat:@"%@:%@:%@:%@:%@", PUBLISHER_TOKEN, [PHAPIRequest session], PHGID(), nonce, PUBLISHER_SECRET],
        *expectedSignature       = [PHStringUtil b64DigestForString:expectedSignatureString];
    STAssertTrue([signature isEqualToString:expectedSignature], @"signature did not match expected value!");

    NSString *nonceParam = [NSString stringWithFormat:@"nonce=%@",nonce];
    STAssertFalse([parameterString rangeOfString:nonceParam].location == NSNotFound,
                  @"Nonce parameter not present!");
}

- (void)testURLProperty
{
    PHAPIRequest *request = [PHAPIRequest requestForApp:PUBLISHER_TOKEN secret:PUBLISHER_SECRET];
    NSString     *desiredURLString = @"http://thisisatesturlstring.com";

    request.urlPath = desiredURLString;
    STAssertFalse([[request.URL absoluteString] rangeOfString:desiredURLString].location == NSNotFound,
                  @"urlPath not present in signed URL!");

}

- (void)testSession
{
    STAssertNoThrow([PHAPIRequest setSession:@"test_session"], @"setting a session shouldn't throw an error");
    STAssertNoThrow([PHAPIRequest setSession:nil], @"clearing a session shouldn't throw");
}
@end

@implementation PHAPIRequestResponseTest

- (void)setUp
{
    _request = [[PHAPIRequest requestForApp:PUBLISHER_TOKEN secret:PUBLISHER_SECRET] retain];
    _request.delegate = self;
    _didProcess = NO;
}

- (void)testResponse
{
    NSDictionary *testDictionary     = [NSDictionary dictionaryWithObjectsAndKeys:
                                                            @"awesomesause", @"awesome", nil];
    NSDictionary *responseDictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                                            testDictionary,@"response",
                                                            [NSNull null],@"error",
                                                            [NSNull null],@"errobj", nil];
    [_request processRequestResponse:responseDictionary];
}

- (void)request:(PHAPIRequest *)request didSucceedWithResponse:(NSDictionary *)responseData
{
    STAssertNotNil(responseData, @"Expected responseData, got nil!");
    STAssertTrue([[responseData allKeys] count] == 1, @"Unexpected number of keys in response data!");
    STAssertTrue([@"awesomesause" isEqualToString:[responseData valueForKey:@"awesome"]],
                 @"Expected 'awesomesause' got %@",
                 [responseData valueForKey:@"awesome"]);
    _didProcess = YES;
}

- (void)request:(PHAPIRequest *)request didFailWithError:(NSError *)error
{
    STFail(@"Request failed with error, but it wasn't supposed to!");
}

- (void)tearDown
{
    STAssertTrue(_didProcess, @"Did not actually process request!");
}

- (void)dealloc
{
    [_request release], _request = nil;
    [super dealloc];
}
@end

@implementation PHAPIRequestErrorTest

- (void)setUp
{
    _request = [[PHAPIRequest requestForApp:PUBLISHER_TOKEN secret:PUBLISHER_SECRET] retain];
    _request.delegate = self;
    _didProcess = NO;
}

- (void)testResponse
{
    NSDictionary *responseDictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                            @"this is awesome!",@"error", nil];
    [_request processRequestResponse:responseDictionary];
}

- (void)request:(PHAPIRequest *)request didSucceedWithResponse:(NSDictionary *)responseData
{
    STFail(@"Request failed succeeded, but it wasn't supposed to!");
}

- (void)request:(PHAPIRequest *)request didFailWithError:(NSError *)error
{
    STAssertNotNil(error, @"Expected error but got nil!");
    _didProcess = YES;
}

- (void)tearDown
{
    STAssertTrue(_didProcess, @"Did not actually process request!");
}
@end

@implementation PHAPIRequestByHashCodeTest

- (void)testRequestByHashCode
{
    int hashCode = 100;

    PHAPIRequest *request = [PHAPIRequest requestForApp:PUBLISHER_TOKEN secret:PUBLISHER_SECRET];
    request.hashCode = hashCode;

    PHAPIRequest *retrievedRequest = [PHAPIRequest requestWithHashCode:hashCode];
    STAssertTrue(request == retrievedRequest, @"Request was not able to be retrieved by hashCode.");
    STAssertNil([PHAPIRequest requestWithHashCode:hashCode+1], @"Non-existent hashCode returned a request.");

    [request cancel];
    STAssertNil([PHAPIRequest requestWithHashCode:hashCode], @"Canceled request was retrieved by hashCode");
}

- (void)testRequestCancelByHashCode
{
    int hashCode = 200;

    PHAPIRequest *request = [PHAPIRequest requestForApp:PUBLISHER_TOKEN secret:PUBLISHER_SECRET];
    request.hashCode = hashCode;

    STAssertTrue([PHAPIRequest cancelRequestWithHashCode:hashCode] == 1, @"Request was not canceled!");
    STAssertTrue([PHAPIRequest cancelRequestWithHashCode:hashCode] == 0, @"Canceled request was canceled again.");
    STAssertTrue([PHAPIRequest cancelRequestWithHashCode:hashCode+1] == 0, @"Nonexistent request was canceled.");
    STAssertFalse([[PHAPIRequest allRequests] containsObject:request], @"Request was not removed from request array!");
}
@end
