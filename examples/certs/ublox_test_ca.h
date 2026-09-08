/*
 * Copyright 2026 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file
 * @brief Self-signed CA certificate for the u-blox HTTPS test rig
 * (EC2 instance used by http_example's optional second download target).
 * Not secret - this is a public root cert used only to validate our own
 * test server's TLS handshake, uploaded to the module via AT+USECUB.
 */

#ifndef _UBLOX_TEST_CA_H_
#define _UBLOX_TEST_CA_H_

static const char gUbloxTestCaCertPem[] =
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "-----END CERTIFICATE-----\n";

#endif // _UBLOX_TEST_CA_H_
